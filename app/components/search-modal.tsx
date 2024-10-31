import { Modal, Pressable } from "react-native";
import { SearchResults } from "./search-results";
import { ListItem, YStack } from "tamagui";

interface Props {
    open: boolean;
    setOpen: (open: boolean) => void;
    onChange: (poi: { id: string; name: string }) => void;
}

export const SearchModal = ({ open, setOpen, onChange }: Props) => {
    return (
        <Modal
            visible={open}
            onRequestClose={() => {
                setOpen(false);
            }}
            animationType="slide"
            presentationStyle="formSheet"
            onDismiss={() => {
                setOpen(false);
            }}
        >
            <YStack backgroundColor="$background" flex={1}>
                <SearchResults>
                    {poi => (
                        <Pressable
                            key={poi.id}
                            onPress={() => {
                                onChange(poi);
                                setOpen(false);
                            }}
                        >
                            <ListItem title={poi.name} />
                        </Pressable>
                    )}
                </SearchResults>
            </YStack>
        </Modal>
    );
};
