import { useQuery } from "@tanstack/react-query";
import { useState } from "react";
import { View, Text } from "react-native";
import { Input, YStack } from "tamagui";

export const SearchResults = ({
    children,
}: {
    children: (poi: { id: string; name: string }) => React.ReactElement;
}) => {
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
        <View>
            <Input
                placeholder="Search"
                m="$2"
                value={search}
                onChangeText={setSearch}
            />
            <YStack>
                {query.isLoading && <Text>Loading...</Text>}
                {query.isSuccess && query.data?.map(poi => children(poi))}
            </YStack>
        </View>
    );
};
