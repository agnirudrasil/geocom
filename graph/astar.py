import heapq

from shapely.geometry.point import Point


# g_cost = distance from start
# h_cost = distance from end (heuristic cost)
# f_cost = g_cost + h_cost

def astar(graph, start, end):
    heap = []
    open_set = set()
    closed = set()

    costs = {}
    start_coords = tuple(map(float, start.split('_')))
    end_coords = tuple(map(float, end.split('_')))
    heapq.heappush(heap, (0, start))
    open_set.add(start)

    while open_set:
        current = heapq.heappop(heap)
        open_set.remove(current[1])
        closed.add(current[1])
        current_coords = tuple(map(float, current[1].split('_')))

        if current[1] == end:
            return retrace_path(costs, start, end)

        for neighbor in graph.neighbors(current[1]):
            if neighbor in closed or not traversable(neighbor):
                continue

            coords = tuple(map(float, neighbor.split('_')))
            cost = costs.get(
                neighbor,
                {
                    'g_cost': distance(start_coords, coords),
                    'h_cost': distance(coords, end_coords),
                    'parent': current[1]
                }
            )
            g_cost = cost['g_cost']
            new_g_cost = cost['g_cost'] + distance(coords, current_coords)
            if new_g_cost < g_cost or neighbor not in open_set:
                costs[neighbor] = {
                    'g_cost': new_g_cost,
                    'h_cost': distance(coords, end_coords),
                    'parent': current[1]
                }

                f_cost = new_g_cost + cost['h_cost']

                if neighbor not in open_set:
                    open_set.add(neighbor)
                    heapq.heappush(heap, (f_cost, neighbor))
                else:
                    for i, (f, n) in enumerate(heap):
                        if n == neighbor:
                            heap[i] = (f_cost, neighbor)
                            break
                    heapq.heapify(heap)


def retrace_path(costs, start, end):
    path = [end]
    current = end
    while current != start:
        current = costs[current]['parent']
        lat_lon = list(map(float, current.split('_')))
        path.append(lat_lon)
    return path[::-1]


def traversable(neighbor):
    return True


def distance(a, b):
    return Point(a).distance(Point(b))
