
#ifndef TILE_MAP_H_
#define TILE_MAP_H_

#include "CommonFunc.h"


class TileMap {
public:
    TileMap();
    ~TileMap();

    // Tải một mảnh bản đồ từ file và đặt nó tại vị trí x
    bool LoadChunk(const std::string& filename, int offset_x);

    // Vẽ phần bản đồ đang hiển thị
    void DrawMap(SDL_Renderer* renderer);

    // Tạo thêm mảnh bản đồ nếu cần dựa vào vị trí người chơi
    void UpdateChunks(int player_x_pos);

    // Lấy giá trị tile tại vị trí cụ thể
    int GetTileValue(int x, int y);

    // Đặt giá trị tile tại vị trí cụ thể
    void SetTileValue(int x, int y, int value);

    // Lấy dữ liệu bản đồ để kiểm tra va chạm
    Map GetMapData() { return map_data; }

private:
    int tile[MAX_MAP_Y][MAX_MAP_X];
    Map map_data;
    int current_rightmost_x; // Theo dõi vị trí xa nhất bên phải đã tải
    std::vector<std::string> chunk_templates; // Các mẫu mảnh bản đồ có sẵn

    // Hàm xóa các tile quá xa phía sau người chơi
    void CleanupOldChunks(int player_x_pos);
};

#endif






#endif
