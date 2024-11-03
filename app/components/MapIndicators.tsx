import { PathfindingAnimation } from "@/types/pathfinding-animation";
import { getAllCoordinates } from "@/utils/rotation";
import { useEffect } from "react";
import MapView, { Marker, Polyline } from "react-native-maps";
import { AnimatedPolyline } from "./animated-polyline";

export const MapIndicators = ({
    index,
    map,
    data,
}: {
    data: PathfindingAnimation;
    index: number;
    map: MapView;
}) => {
    useEffect(() => {
        let coordinates;
        if (index === data.steps.length - 1) {
            coordinates = getAllCoordinates({ ...data, steps: [] });
        } else {
            coordinates = getAllCoordinates({
                ...data,
                steps: data.steps.slice(0, index + 1),
            });
        }

        map.fitToCoordinates(coordinates, {
            edgePadding: { top: 30, right: 30, bottom: 100, left: 30 },
        });
    }, [index]);
    return (
        <>
            <Marker
                coordinate={{ latitude: data.from[0], longitude: data.from[1] }}
            />
            <Marker
                coordinate={{ latitude: data.to[0], longitude: data.to[1] }}
            />
            {data.steps.slice(0, index + 1).map((step, i) =>
                step.children.map((child, j) => (
                    <Polyline
                        key={`${i}-${j}`}
                        strokeWidth={4}
                        strokeColor="rgba(96, 125, 139, 0.7)"
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
            <AnimatedPolyline
                play={index >= data.steps.length - 1}
                strokeWidth={6}
                strokeColor="#81c784"
                coordinates={data.path.map((point: number[]) => ({
                    latitude: point[0],
                    longitude: point[1],
                }))}
            />
        </>
    );
};
