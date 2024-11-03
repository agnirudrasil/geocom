import { Animated, StyleSheet, useAnimatedValue } from "react-native";

import { View } from "@/components/Themed";
import MapView from "react-native-maps";
import { useEffect, useRef, useState } from "react";
import {
    useTheme,
    H2,
    YStack,
    Button,
    YGroup,
    Separator,
    XStack,
    Label,
} from "tamagui";
import BottomSheet, { BottomSheetView } from "@gorhom/bottom-sheet";
import { MapPin, Navigation } from "@tamagui/lucide-icons";
import { useQuery } from "@tanstack/react-query";
import { useLocalSearchParams } from "expo-router";
import { SearchModal } from "@/components/search-modal";
import { MapIndicators } from "@/components/MapIndicators";
import { PathfindingAnimation } from "@/types/pathfinding-animation";
import { Dropdown } from "@/components/select";

interface IdName {
    id: string;
    name: string;
}

export default function TabOneScreen() {
    const [position, setPosition] = useState(0);
    const [algo, setAlgo] = useState("astar");
    const theme = useTheme();
    const { poi } = useLocalSearchParams<{ poi?: string }>();

    const mapRef = useRef<MapView>(null);
    const sheetRef = useRef<BottomSheet>(null);
    const [from, setFrom] = useState<IdName | null>(null);
    const [to, setTo] = useState<IdName | null>(
        poi ? (JSON.parse(poi) as IdName) : null
    );

    const { data } = useQuery({
        queryKey: ["directions", from, to, algo],
        enabled: Boolean(from) && Boolean(to),
        queryFn: async ({ queryKey }) => {
            if (!queryKey[1] || !queryKey[2]) {
                return;
            }
            const response = await fetch(
                `${process.env.EXPO_PUBLIC_API_URL}/path?from=${
                    (queryKey[1] as IdName).id
                }&to=${(queryKey[2] as IdName).id}&algo=${queryKey[3]}`
            );
            if (!response.ok) {
                throw new Error("Network response was not ok");
            }
            return response.json() as Promise<PathfindingAnimation>;
        },
    });

    const [open, setOpen] = useState<"from" | "to" | null>(null);
    const animatedIndex = useAnimatedValue(0);
    const [index, setIndex] = useState(0);
    const animationRef = useRef<Animated.CompositeAnimation | null>(null);

    useEffect(() => {
        if (data)
            animationRef.current = Animated.timing(animatedIndex, {
                toValue: data.steps.length - 1,
                duration: 5000,
                useNativeDriver: true,
            });
    }, [animatedIndex, data]);

    useEffect(() => {
        animatedIndex.addListener(({ value }) => {
            setIndex(value);
        });

        return () => {
            animatedIndex.removeAllListeners();
        };
    }, [animatedIndex]);

    useEffect(() => {
        if (data) {
            const allCoordinates = [
                ...data.path.map(([lat, lon]) => ({
                    latitude: lat,
                    longitude: lon,
                })),
            ];
            mapRef.current?.fitToCoordinates(allCoordinates, {
                edgePadding: {
                    top: 30,
                    bottom: 100,
                    left: 30,
                    right: 30,
                },
                animated: true,
            });
        } else {
            sheetRef.current?.expand();
        }
    }, [data]);

    return (
        <View>
            <SearchModal
                onChange={poi => {
                    if (open === "from") {
                        setFrom(poi);
                    } else {
                        setTo(poi);
                    }
                }}
                open={Boolean(open)}
                setOpen={() => setOpen(null)}
            />
            <MapView
                ref={mapRef}
                style={styles.map}
                onLayout={() => {
                    if (data) {
                        console.log("Running");
                        mapRef.current?.fitToCoordinates(
                            data.path.map(([lat, lon]) => ({
                                latitude: lat,
                                longitude: lon,
                            })),
                            {
                                animated: true,
                            }
                        );
                    }
                }}
            >
                {data && (
                    <MapIndicators
                        map={mapRef.current!}
                        index={index}
                        data={data}
                    />
                )}
            </MapView>
            <BottomSheet
                backgroundStyle={{
                    backgroundColor: theme.background.val,
                }}
                handleIndicatorStyle={{
                    backgroundColor: theme.accentColor.val,
                }}
                snapPoints={["15%", "50%"]}
                index={1}
                onChange={index => setPosition(index)}
                enableDynamicSizing={false}
                ref={sheetRef}
            >
                <BottomSheetView>
                    <YStack padding="$3" gap="$4">
                        <H2>Directions</H2>
                        {position > 0 && (
                            <YStack gap="$2">
                                <YGroup
                                    borderRadius="$2"
                                    padding="$1.5"
                                    paddingStart="$2"
                                    backgroundColor="$accentBackground"
                                    separator={
                                        <Separator borderColor="$accentColor" />
                                    }
                                >
                                    <YGroup.Item>
                                        <Button
                                            chromeless
                                            jc="flex-start"
                                            icon={Navigation}
                                            width="100%"
                                            onPress={() => setOpen("from")}
                                        >
                                            {from ? from.name : "From"}
                                        </Button>
                                    </YGroup.Item>
                                    <YGroup.Item>
                                        <Button
                                            icon={MapPin}
                                            jc="flex-start"
                                            chromeless
                                            width="100%"
                                            onPress={() => setOpen("to")}
                                        >
                                            {to ? to.name : "From"}
                                        </Button>
                                    </YGroup.Item>
                                </YGroup>
                                <Button
                                    onPress={() => {
                                        sheetRef.current?.collapse();
                                        animationRef.current?.reset();
                                        animationRef.current?.start();
                                    }}
                                    theme="green"
                                    width="100%"
                                >
                                    Go!
                                </Button>
                                <XStack>
                                    <Label f={1} miw={80}>
                                        Algorithm
                                    </Label>
                                    <Dropdown
                                        value={algo}
                                        onValueChange={setAlgo}
                                        items={[
                                            {
                                                name: "A*",
                                                value: "astar",
                                            },
                                            {
                                                name: "Dijkstra",
                                                value: "dijkstra",
                                            },
                                        ]}
                                    />
                                </XStack>
                            </YStack>
                        )}
                    </YStack>
                </BottomSheetView>
            </BottomSheet>
        </View>
    );
}

const styles = StyleSheet.create({
    map: {
        width: "100%",
        height: "100%",
    },
});
