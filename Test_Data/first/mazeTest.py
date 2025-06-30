import json
from collections import deque
import itertools

# 从JSON文件中读取迷宫
def read_maze(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
    return data['maze']

# 广度优先搜索函数
def bfs(maze, start, end=None):
    rows, cols = len(maze), len(maze[0])
    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]
    visited = [[False] * cols for _ in range(rows)]
    visited[start[0]][start[1]] = True
    queue = deque([(start, [start])])
    paths = []

    while queue:
        (x, y), path = queue.popleft()
        if end and (x, y) == end:
            paths.append(path)
            continue
        for dx, dy in directions:
            new_x, new_y = x + dx, y + dy
            if 0 <= new_x < rows and 0 <= new_y < cols and maze[new_x][new_y] != '#' and not visited[new_x][new_y]:
                visited[new_x][new_y] = True
                new_path = path + [(new_x, new_y)]
                queue.append(((new_x, new_y), new_path))

    return paths, visited

# 使用DFS检测迷宫中是否存在环
def detect_cycle(maze):
    rows, cols = len(maze), len(maze[0])
    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]
    visited = [[False] * cols for _ in range(rows)]
    parent = {}  # 记录每个节点的父节点
    cycles = []  # 存储找到的环
    
    def dfs(x, y, parent_pos=None):
        visited[x][y] = True
        current_pos = (x, y)
        parent[current_pos] = parent_pos
        
        for dx, dy in directions:
            new_x, new_y = x + dx, y + dy
            new_pos = (new_x, new_y)
            
            # 检查是否在迷宫范围内且是通道
            if 0 <= new_x < rows and 0 <= new_y < cols and maze[new_x][new_y] != '#':
                if not visited[new_x][new_y]:
                    if dfs(new_x, new_y, current_pos):
                        return True
                # 如果邻居已访问且不是父节点，则找到环
                elif parent_pos != new_pos:
                    # 重建环路径
                    cycle = [current_pos]
                    back_pos = current_pos
                    while back_pos != new_pos:
                        back_pos = parent[back_pos]
                        if back_pos is None:  # 防止意外情况
                            break
                        cycle.append(back_pos)
                    cycle.append(new_pos)
                    cycles.append(cycle)
                    return True
        return False
    
    # 从每个未访问的通道开始DFS
    for i in range(rows):
        for j in range(cols):
            if maze[i][j] != '#' and not visited[i][j]:
                if dfs(i, j):
                    return True, cycles
    
    return False, cycles

# 判断两点间是否有唯一通路
def has_unique_path(maze, start, end):
    paths, _ = bfs(maze, start, end)
    if not paths:  # 如果没有路径，则不存在唯一通路
        return False
    return len(paths) == 1

# 检查迷宫中任意两点间是否都是唯一通路
def check_all_pairs_unique_paths(maze, passages):
    # 首先检测迷宫中是否存在环
    has_cycle, cycles = detect_cycle(maze)
    
    if has_cycle:
        # 如果存在环，则必定存在不唯一的通路
        return False, 0, len(list(itertools.combinations(passages, 2))), cycles
    else:
        # 如果不存在环，则所有点对之间都是唯一通路
        total_pairs = len(list(itertools.combinations(passages, 2)))
        return True, total_pairs, total_pairs, []

# 找到起点和终点的坐标以及所有可通行的格子
def find_start_end_and_passages(maze):
    start = None
    end = None
    passages = []
    
    for i in range(len(maze)):
        for j in range(len(maze[0])):
            cell = maze[i][j]
            if cell == 'S':
                start = (i, j)
                passages.append((i, j))
            elif cell == 'E':
                end = (i, j)
                passages.append((i, j))
            elif cell != '#':  # 非墙壁即为通道
                passages.append((i, j))
                
    return start, end, passages

# 检测迷宫中的孤立区域
def find_isolated_areas(maze, start):
    rows, cols = len(maze), len(maze[0])
    
    # 从起点开始BFS，标记所有可达的格子
    _, visited_from_start = bfs(maze, start)
    
    # 查找所有非墙壁但不可达的格子
    isolated_areas = []
    
    for i in range(rows):
        for j in range(cols):
            if maze[i][j] != '#' and not visited_from_start[i][j]:
                isolated_areas.append((i, j))
    
    return isolated_areas

# 检查终点是否可达
def is_end_reachable(maze, start, end):
    paths, _ = bfs(maze, start, end)
    return len(paths) > 0

# 主函数
def main():
    file_path = 'maze2.json'  # 迷宫文件路径
    maze = read_maze(file_path)
    start, end, passages = find_start_end_and_passages(maze)
    
    if start and end:
        # 检查终点是否可达
        end_reachable = is_end_reachable(maze, start, end)
        if not end_reachable:
            print("终点不可达！")
        else:
            # 检查是否有唯一通路
            unique_path = has_unique_path(maze, start, end)
            print(f"迷宫中从起点到终点是否有唯一通路: {unique_path}")
        
        # 检查是否有孤立区域
        isolated_areas = find_isolated_areas(maze, start)
        
        if isolated_areas:
            # 检查终点是否在孤立区域中
            end_isolated = end in isolated_areas
            if end_isolated:
                print(f"终点 {end} 不可从起点到达！")
                # 移除终点，只显示其他孤立区域
                isolated_areas.remove(end)
            
            if isolated_areas:  # 如果还有其他孤立区域
                print(f"迷宫中存在孤立区域，共有 {len(isolated_areas)} 个格子不可达:")
                for i, area in enumerate(isolated_areas[:5]):  # 只显示前5个孤立格子
                    print(f"  - 孤立格子 {i+1}: 坐标 {area}, 内容: {maze[area[0]][area[1]]}")
                if len(isolated_areas) > 5:
                    print(f"  ... 以及其他 {len(isolated_areas) - 5} 个孤立格子")
            else:
                print("除了终点外，迷宫中不存在其他孤立区域。")
        else:
            print("迷宫中不存在孤立区域，所有非墙壁格子都可以从起点到达。")
            
            # 检查迷宫中是否存在环，从而判断任意两点间是否都是唯一通路
            all_unique, unique_count, total_count, cycles = check_all_pairs_unique_paths(maze, passages)
            
            if all_unique:
                print(f"迷宫中不存在环，所有点对之间都有唯一通路，共有 {total_count} 对点。")
            else:
                print(f"迷宫中存在环，因此并非所有点对之间都有唯一通路。")
                if cycles:
                    print(f"发现的环路径示例:")
                    for i, cycle in enumerate(cycles[:3]):
                        print(f"  - 环 {i+1}: {' -> '.join(str(pos) for pos in cycle)}")
                    if len(cycles) > 3:
                        print(f"  ... 以及其他 {len(cycles) - 3} 个环")
    else:
        print("未找到起点或终点。")

if __name__ == "__main__":
    main()