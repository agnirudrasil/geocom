import { PathfindingAnimation } from "@/types/pathfinding-animation";

export const rotation = (point: number[], path: number[][]) => {
    for (let i = 1; i < path.length; i++) {
        if (path[i][0] < point[0] && path[i - 1][0] > point[0]) {
            const prev = path[i - 1];
            const next = path[i + 1];
            // returns rotation in degrees
            return (
                (Math.atan2(next[1] - prev[1], next[0] - prev[0]) * 180) /
                Math.PI
            );
        }
    }
};

export const getAllCoordinates = (data: PathfindingAnimation) => {
    return [
        ...data.path.map(([lat, lon]) => ({
            latitude: lat,
            longitude: lon,
        })),
        ...data.steps.map(step => ({
            latitude: step.parent[0],
            longitude: step.parent[1],
        })),
        ...data.steps.flatMap(step =>
            step.children.map(child => ({
                latitude: child[0],
                longitude: child[1],
            }))
        ),
    ];
};
