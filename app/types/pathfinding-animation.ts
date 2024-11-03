import { LineString, Point } from "./shapes";

export interface PathfindingAnimation {
    path: LineString;
    step_count: number;
    steps: {
        parent: Point;
        children: Point[];
    }[];
    from: Point;
    to: Point;
}
