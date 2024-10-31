import { StyleSheet } from "react-native";

import { View } from "@/components/Themed";
import MapView, { Geojson, Marker } from "react-native-maps";
import { useEffect, useRef, useState } from "react";
import { useTheme, H2, YStack, Button, YGroup, Separator } from "tamagui";
import BottomSheet, { BottomSheetView } from "@gorhom/bottom-sheet";
import { MapPin, Navigation } from "@tamagui/lucide-icons";
import { useQuery } from "@tanstack/react-query";
import { useLocalSearchParams } from "expo-router";
import { SearchModal } from "@/components/search-modal";

interface IdName {
    id: string;
    name: string;
}

export default function TabOneScreen() {
    const [position, setPosition] = useState(0);
    const theme = useTheme();
    const { poi } = useLocalSearchParams<{ poi?: string }>();

    const mapRef = useRef<MapView>(null);
    const sheetRef = useRef<BottomSheet>(null);
    const [from, setFrom] = useState<IdName | null>(null);
    const [to, setTo] = useState<IdName | null>(
        poi ? (JSON.parse(poi) as IdName) : null
    );

    const { data } = useQuery({
        queryKey: ["directions", from, to],
        enabled: Boolean(from) && Boolean(to),
        queryFn: async ({ queryKey }) => {
            if (!queryKey[1] || !queryKey[2]) {
                return;
            }
            const response = await fetch(
                `${process.env.EXPO_PUBLIC_API_URL}/path?from=${
                    (queryKey[1] as IdName).id
                }&to=${(queryKey[2] as IdName).id}`
            );
            return response.json() as Promise<{
                path: number[][];
                from: number[];
                to: number[];
            }>;
        },
    });

    const [open, setOpen] = useState<"from" | "to" | null>(null);

    useEffect(() => {
        setTimeout(() => {
            sheetRef.current?.expand();
        }, 10);
    }, []);

    useEffect(() => {
        if (data) {
            console.log(data);
            const minLat = Math.min(...data.path.map(c => c[0]));
            const maxLat = Math.max(...data.path.map(c => c[0]));
            const maxLon = Math.max(...data.path.map(c => c[1]));
            const minLon = Math.min(...data.path.map(c => c[1]));
            mapRef.current?.animateToRegion({
                latitude: (minLat + maxLat) / 2,
                longitude: (minLon + maxLon) / 2,
                latitudeDelta: Math.abs(minLat - maxLat) * 1.5,
                longitudeDelta: Math.abs(minLon - maxLon) * 1.25,
            });
            sheetRef.current?.collapse();
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
            <MapView ref={mapRef} style={styles.map}>
                {data && (
                    <Marker
                        coordinate={{
                            latitude: data.from[0],
                            longitude: data.from[1],
                        }}
                    />
                )}
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
                        strokeWidth={3}
                        strokeColor="#00f"
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
                            <>
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
                                <Button theme="green" width="100%">
                                    Go!
                                </Button>
                            </>
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
