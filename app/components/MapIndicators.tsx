import {
    Accuracy,
    LocationObjectCoords,
    LocationSubscription,
    requestForegroundPermissionsAsync,
    watchPositionAsync,
} from "expo-location";
import { useEffect, useState } from "react";
import { Marker, Polyline } from "react-native-maps";
import data from "@/app/animation.json";

export const MapIndicators = ({
    index,
}: {
    data: {
        from: number[];
        to: number[];
        path: number[][];
    };
    index: number;
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
            <Marker
                coordinate={{
                    latitude: 13.0095455,
                    longitude: 74.7950478,
                }}
            />
            {data.steps.slice(0, index + 1).map((step, i) =>
                step.children.map((child, j) => (
                    <Polyline
                        strokeWidth={4}
                        strokeColor="red"
                        coordinates={[
                            {
                                latitude: step.parent[0],
                                longitude: step.parent[1],
                            },
                            {
                                latitude: child[0],
                                longitude: child[1],
                            },
                        ]}
                    />
                ))
            )}
            {data && data.path.length && index >= 57 && (
                <Polyline
                    strokeWidth={6}
                    strokeColor="#4CAF50"
                    coordinates={data.path.map((point: number[]) => ({
                        latitude: point[0],
                        longitude: point[1],
                    }))}
                />
            )}
        </>
    );
};
