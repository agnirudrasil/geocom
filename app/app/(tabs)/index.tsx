import { StyleSheet } from "react-native";

import { View } from "@/components/Themed";
import MapView, { Geojson, Marker, Region, UrlTile } from "react-native-maps";
import elements from "./map.json";
import { useEffect, useState } from "react";

export default function TabOneScreen() {
  const [region, setRegion] = useState({
    latitude: 13.01086121451261,
    longitude: 74.79429826652435,
    latitudeDelta: 0.016103154323193536,
    longitudeDelta: 0.009563403566900774,
  });

  const markers = [
    [13.0124929, 74.7930409],
    [13.0124924, 74.7930925],
    [13.0124815, 74.7941749],
    [13.0120704, 74.7941659],
    [13.0117955, 74.7941598],
    [13.0115564, 74.7941763],
    [13.0114864, 74.7941763],
    [13.0114761, 74.7943094],
    [13.0114735, 74.7943671],
    [13.0114813, 74.7944301],
    [13.0114238, 74.7944391],
    [13.011368, 74.7944376],
    [13.0112778, 74.7944439],
    [13.0112548, 74.7944556],
    [13.0110175, 74.7944627],
    [13.0108711, 74.7944646],
    [13.0106072, 74.7944681],
    [13.0105006, 74.794474],
    [13.0104457, 74.7945276],
    [13.0103516, 74.7946966],
    [13.0102784, 74.7948415],
    [13.0102053, 74.7950104],
    [13.0101881, 74.7950305],
    [13.0101532, 74.7950433],
    [13.0098255, 74.7950354],
    [13.0095455, 74.7950478],
    [13.009143, 74.7950426],
    [13.0091495, 74.7954087],
    [13.009154, 74.7958513],
    [13.009155, 74.7959545],
    [13.0091351, 74.7965667],
    [13.0089608, 74.7966621],
    [13.0088534, 74.7966654],
    [13.008781, 74.7966707],
    [13.0085981, 74.7967029],
    [13.0083509, 74.7968071],
    [13.0083054, 74.7968263],
    [13.0081799, 74.7968987],
    [13.0081069, 74.7969243],
    [13.0080513, 74.7969438],
    [13.0079499, 74.7969738],
    [13.0077878, 74.7969855],
    [13.0076886, 74.7969926],
    [13.0075553, 74.7969845],
    [13.0074064, 74.7969336],
    [13.0073854, 74.7969899],
    [13.0073358, 74.7970543],
    [13.0072809, 74.7970999],
  ];

  return (
    <View style={styles.container}>
      <MapView initialRegion={region} style={styles.map}>
        <Geojson
          strokeWidth={3}
          strokeColor="red"
          geojson={{
            features: [
              {
                properties: {
                  type: "way",
                },
                type: "Feature",
                geometry: {
                  type: "LineString",
                  coordinates: markers.map((marker) => marker.reverse()),
                },
              },
            ],
          }}
        />
      </MapView>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: "center",
    justifyContent: "center",
  },
  title: {
    fontSize: 20,
    fontWeight: "bold",
  },
  separator: {
    marginVertical: 30,
    height: 1,
    width: "80%",
  },

  map: {
    width: "100%",
    height: "100%",
  },
});
