import { useEffect, useRef, useState } from "react";
import { Animated, useAnimatedValue } from "react-native";
import { MapPolylineProps, Polyline } from "react-native-maps";

export const AnimatedPolyline = ({
    coordinates,
    play,
    ...props
}: MapPolylineProps & { play: boolean }) => {
    const animatedIndex = useAnimatedValue(0);
    const [index, setIndex] = useState(0);
    const animation = useRef<Animated.CompositeAnimation | null>(null);

    useEffect(() => {
        animation.current = Animated.timing(animatedIndex, {
            toValue: coordinates.length - 1,
            duration: 1000,
            useNativeDriver: true,
        });
    }, [animatedIndex, coordinates]);

    useEffect(() => {
        animatedIndex.addListener(({ value }) => {
            setIndex(value);
        });
        return () => {
            animatedIndex.removeAllListeners();
        };
    }, [animatedIndex]);

    useEffect(() => {
        if (play) {
            animation.current?.reset();
            animation.current?.start();
        } else {
            animation.current?.stop();
            animation.current?.reset();
        }
    }, [play]);

    return (
        <Polyline coordinates={coordinates.slice(0, index + 1)} {...props} />
    );
};
