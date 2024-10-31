import {
    Keyboard,
    StyleSheet,
    Text,
    TouchableOpacity,
    TouchableWithoutFeedback,
} from "react-native";

import { View } from "@/components/Themed";
import { Input, ListItem, YStack } from "tamagui";
import { useQuery } from "@tanstack/react-query";
import { useState } from "react";
import { Link } from "expo-router";

export default function TabOneScreen() {
    const [search, setSearch] = useState("");
    const query = useQuery({
        queryKey: ["pois", search],
        queryFn: async ({ queryKey }) => {
            const query = queryKey[1];
            if (query.length > 2) {
                const response = await fetch(
                    `${process.env.EXPO_PUBLIC_API_URL}/pois/?q=${query}`
                );
                const data = response.json() as Promise<
                    {
                        id: string;
                        name: string;
                    }[]
                >;
                return data;
            }
            return null;
        },
    });

    return (
        <TouchableWithoutFeedback
            style={{ backgroundColor: "white", height: "100%" }}
            onPress={Keyboard.dismiss}
        >
            <View>
                <Input
                    placeholder="Search"
                    m="$2"
                    value={search}
                    onChangeText={setSearch}
                />
                <YStack>
                    {query.isLoading && <Text>Loading...</Text>}
                    {query.isSuccess && (
                        <>
                            {query.data?.map(poi => (
                                <Link
                                    key={poi.id}
                                    href={`/modal?poi=${poi.id}`}
                                >
                                    <ListItem title={poi.name} />
                                </Link>
                            ))}
                        </>
                    )}
                </YStack>
            </View>
        </TouchableWithoutFeedback>
    );
}

const styles = StyleSheet.create({
    map: {
        width: "100%",
        height: "100%",
    },
});
