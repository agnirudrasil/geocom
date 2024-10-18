import json

import geojson
import matplotlib.pyplot as plt
import networkx as nx
from pyvis.network import Network
from scipy.constants import point
from shapely import LineString, get_coordinates, distance, Point
from shapely.ops import nearest_points

from graph.astar import astar

JSON_PATH = "data/geojson/roads.json"

with open(JSON_PATH) as f:
    features = json.load(f)

filtered_features = list(filter(
    # lambda x: x['properties']['id'] in {176069414, 176069651, 176069415,
    #                                     176069514, 1220209675,
    #                                     1220209675, 176069361
    #                                     } and x['geometry']['type'] == 'LineString',
    lambda x: x['geometry']['type'] == 'LineString',
    features['features']
))

graph = nx.Graph()


def point_to_id(intersect):
    return f"{intersect[1]}_{intersect[0]}"


for feature in filtered_features:
    prev_coords = feature['geometry']['coordinates'][0]
    node = point_to_id(prev_coords)
    graph.add_node(node)
    for coords in feature['geometry']['coordinates'][1:]:
        curr = point_to_id(coords)
        graph.add_node(curr)
        graph.add_edge(node, curr)
        node = curr

# net = Network(
#     directed=False,
# )
# net.show_buttons()  # Show part 3 in the plot (optional)
# net.from_nx(graph)  # Create directly from nx graph
# net.show('test.html', notebook=False)


print(
    astar(
        graph,
        point_to_id((74.7930409, 13.0124929)),
        point_to_id((74.7970999, 13.0072809))
    )
)
# ('13.0114864_74.7941763', '13.0088370_74.7954886')
