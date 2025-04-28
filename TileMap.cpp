#include "TileMap.h"

TileMap::TileMap() {
    // Khởi tạo mảng tile toàn số 0
    for (int i = 0; i < MAX_MAP_Y; i++) {
        for (int j = 0; j < MAX_MAP_X; j++) {
            tile[i][j] = 0;
        }
    }

    current_rightmost_x = 0;

    // Thêm các mẫu mảnh bản đồ có sẵn
    chunk_templates.push_back("chunk01.dat");
    chunk_templates.push_back("chunk02.dat");
    chunk_templates.push_back("chunk03.dat");
    chunk_templates.push_back("chunk04.dat");
    chunk_templates.push_back("chunk05.dat");

    // Khởi tạo với một nền tảng ban đầu
    LoadChunk("chunk01.dat", 0); // Mảnh bản đồ bắt đầu
}

TileMap::~TileMap() {
    // Dọn dẹp nếu cần
}

bool TileMap::LoadChunk(const std::string& filename, int offset_x) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Cập nhật vị trí mảnh xa nhất bên phải nếu cần
    int chunk_width = 20; // Giả sử mỗi mảnh rộng 20 tile
    if (offset_x + chunk_width > current_rightmost_x) {
        current_rightmost_x = offset_x + chunk_width;
    }

    // Tải dữ liệu mảnh
    for (int row = 0; row < 10; ++row) { // Giả sử mảnh cao 10 tile
        for (int col = 0; col < chunk_width; ++col) {
            int value;
            file >> value;

            // Đảm bảo không vượt quá giới hạn
            if ((col + offset_x) < MAX_MAP_X) {
                tile[row][col + offset_x] = value;
            }
        }
    }

    file.close();
    return true;
}

void TileMap::DrawMap(SDL_Renderer* renderer) {
    // Tính toán phạm vi hiển thị dựa trên vị trí camera
    int start_x = map_data.start_x_ / TILE_SIZE;
    int end_x = (map_data.start_x_ + SCREEN_WIDTH) / TILE_SIZE + 1;

    // Đảm bảo không vượt quá giới hạn
    start_x = std::max(0, start_x);
    end_x = std::min(MAX_MAP_X - 1, end_x);

    // Vẽ các tile đang hiển thị
    for (int row = 0; row < MAX_MAP_Y; ++row) {
        for (int col = start_x; col <= end_x; ++col) {
            if (tile[row][col] > 0 && tile[row][col] < MAX_TILES) {
                // Tính toán vị trí trên màn hình
                int x_pos = col * TILE_SIZE - map_data.start_x_;
                int y_pos = row * TILE_SIZE - map_data.start_y_;

                SDL_Rect dest_rect = {x_pos, y_pos, TILE_SIZE, TILE_SIZE};

                // Vẽ tile sử dụng bảng tile và renderer của bạn
                // Bạn cần điều chỉnh phần này phù hợp với phương thức vẽ cụ thể của bạn
                // Ví dụ:
                // game_map.RenderTile(tile[row][col], dest_rect, renderer);
            }
        }
    }
}

void TileMap::UpdateChunks(int player_x_pos) {
    // Chuyển đổi vị trí người chơi sang tọa độ tile
    int player_tile_x = player_x_pos / TILE_SIZE;

    // Kiểm tra xem có cần tạo thêm mảnh bản đồ không
    // Nếu người chơi gần đến mảnh xa nhất bên phải (trong vòng 60 tile)
    if (player_tile_x > current_rightmost_x - 60) {
        // Tạo một mảnh ngẫu nhiên mới
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, chunk_templates.size() - 1);

        int random_index = dist(gen);
        std::string random_chunk = chunk_templates[random_index];

        // Tải mảnh mới tại vị trí xa nhất bên phải
        LoadChunk(random_chunk, current_rightmost_x);
    }

    // Dọn dẹp các mảnh cũ quá xa phía sau
    CleanupOldChunks(player_x_pos);
}

void TileMap::CleanupOldChunks(int player_x_pos) {
    // Chuyển đổi vị trí người chơi sang tọa độ tile
    int player_tile_x = player_x_pos / TILE_SIZE;

    // Định nghĩa "quá xa phía sau" là 100 tile bên trái người chơi
    int cleanup_x = player_tile_x - 100;

    // Không dọn dẹp nếu chúng ta vẫn ở đầu bản đồ
    if (cleanup_x <= 0) return;

    // Xóa các tile quá xa bên trái
    for (int row = 0; row < MAX_MAP_Y; ++row) {
        for (int col = 0; col < cleanup_x; ++col) {
            tile[row][col] = 0;
        }
    }
}

int TileMap::GetTileValue(int x, int y) {
    if (x < 0 || x >= MAX_MAP_X || y < 0 || y >= MAX_MAP_Y) {
        return 0; // Ngoài giới hạn
    }
    return tile[y][x];
}

void TileMap::SetTileValue(int x, int y, int value) {
    if (x < 0 || x >= MAX_MAP_X || y < 0 || y >= MAX_MAP_Y) {
        return; // Ngoài giới hạn
    }
    tile[y][x] = value;
}
