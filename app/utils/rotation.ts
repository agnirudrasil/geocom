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
