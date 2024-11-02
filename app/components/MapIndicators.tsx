import {
    Accuracy,
    LocationObjectCoords,
    LocationSubscription,
    requestForegroundPermissionsAsync,
    watchPositionAsync,
} from "expo-location";
import { useEffect, useState } from "react";
import { Marker, Geojson } from "react-native-maps";

export const MapIndicators = ({
    data,
}: {
    data: any;
    setRegion: (coords: LocationObjectCoords) => any;
}) => {
    const [location, setLocation] = useState<LocationObjectCoords | null>(null);

    useEffect(() => {
        let subscription: LocationSubscription | null = null;

        (async () => {
            const { status } = await requestForegroundPermissionsAsync();
            if (status === "granted") {
                subscription = await watchPositionAsync(
                    { accuracy: Accuracy.BestForNavigation },
                    ({ coords }) => {
                        setLocation(coords);
                    }
                );
            } else {
                console.log("Location permissions not granted");
            }
        })();

        return () => {
            subscription?.remove();
        };
    }, []);

    return (
        <>
            {data && (
                <Marker
                    coordinate={{
                        latitude: data.to[0],
                        longitude: data.to[1],
                    }}
                />
            )}
            {data && data.path.length && (
                <Geojson
                    strokeWidth={5}
                    strokeColor="#4CAF50"
                    geojson={{
                        type: "FeatureCollection",
                        features: [
                            {
                                type: "Feature",
                                properties: {
                                    id: "line1",
                                },
                                geometry: {
                                    type: "LineString",
                                    coordinates: data.path.map(c =>
                                        c.toReversed()
                                    ),
                                },
                            },
                        ],
                    }}
                />
            )}
            {location && (
                <Marker
                    coordinate={{
                        latitude: location.latitude,
                        longitude: location.longitude,
                    }}
                    flat
                    image={require("../assets/images/pin.png")}
                />
            )}
        </>
    );
};
