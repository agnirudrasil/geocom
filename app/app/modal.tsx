import { StatusBar } from "expo-status-bar";
import { Platform, StyleSheet } from "react-native";

import { View } from "@/components/Themed";
import { Stack, useLocalSearchParams, useRouter } from "expo-router";
import { useQuery } from "@tanstack/react-query";
import {
    Button,
    H6,
    ListItem,
    ListItemSubtitle,
    ListItemText,
    Paragraph,
    SizableText,
    YStack,
} from "tamagui";

export default function ModalScreen() {
    const { poi } = useLocalSearchParams<{ poi?: string }>();
    const { data } = useQuery({
        queryKey: ["poi", poi],
        queryFn: async ({ queryKey }) => {
            const response = await fetch(
                `${process.env.EXPO_PUBLIC_API_URL}/pois/${queryKey[1]}`
            );
            const data = response.json() as Promise<{
                id: string;
                name: string;
                description: string;
                rrule: string;
                pocs: Array<{
                    name: string;
                    phone: string;
                    email: string;
                }>;
            }>;
            return data;
        },
    });

    const router = useRouter();

    return (
        <View style={styles.container}>
            <Stack.Screen
                options={{ title: data?.name, presentation: "modal" }}
            />
            <YStack p="$3" gap="$3">
                <ListItem title={data?.name} subTitle={data?.description} />
                <YStack>
                    <H6>Timing</H6>
                    <Paragraph>{data?.rrule}</Paragraph>
                </YStack>
                <H6>Points of Contacts</H6>
                <YStack flexGrow={1}>
                    {data?.pocs.map(poc => (
                        <ListItem key={poc.name}>
                            <ListItemText>
                                <SizableText>{poc.name}</SizableText>
                            </ListItemText>
                            <ListItemSubtitle>{poc.phone}</ListItemSubtitle>
                            <ListItemSubtitle>{poc.email}</ListItemSubtitle>
                        </ListItem>
                    ))}
                </YStack>
                <Button
                    onPress={() => {
                        router.dismiss();
                        router.push(
                            `/map?poi=${encodeURIComponent(
                                JSON.stringify({
                                    id: data?.id,
                                    name: data?.name,
                                })
                            )}`
                        );
                    }}
                    theme="green"
                >
                    Directions
                </Button>
            </YStack>
            {/* Use a light status bar on iOS to account for the black space above the modal */}
            <StatusBar style={Platform.OS === "ios" ? "light" : "auto"} />
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
    },
    link: { width: "100%" },
    separator: {},
});
