import React from "react";
import { Check, ChevronDown, ChevronUp } from "@tamagui/lucide-icons";

import type { FontSizeTokens, SelectProps } from "tamagui";
import { Adapt, Select, Sheet, YStack, getFontSize } from "tamagui";

export function Dropdown({
    items,
    ...props
}: SelectProps & { items: { name: string; value: string }[] }) {
    return (
        <Select disablePreventBodyScroll {...props}>
            <Select.Trigger width={220} iconAfter={ChevronDown}>
                <Select.Value placeholder="Something" />
            </Select.Trigger>
            <Adapt when="sm" platform="touch">
                <Sheet
                    native={!!props.native}
                    modal
                    dismissOnSnapToBottom
                    animationConfig={{
                        type: "spring",
                        damping: 20,
                        mass: 1.2,
                        stiffness: 250,
                    }}
                >
                    <Sheet.Frame>
                        <Sheet.ScrollView>
                            <Adapt.Contents />
                        </Sheet.ScrollView>
                    </Sheet.Frame>
                    <Sheet.Overlay
                        animation="lazy"
                        enterStyle={{ opacity: 0 }}
                        exitStyle={{ opacity: 0 }}
                    />
                </Sheet>
            </Adapt>

            <Select.Content zIndex={200000}>
                <Select.ScrollUpButton
                    alignItems="center"
                    justifyContent="center"
                    position="relative"
                    width="100%"
                    height="$3"
                >
                    <YStack zIndex={10}>
                        <ChevronUp size={20} />
                    </YStack>
                </Select.ScrollUpButton>

                <Select.Viewport minWidth={200}>
                    <Select.Group>
                        {/* for longer lists memoizing these is useful */}
                        {React.useMemo(
                            () =>
                                items.map((item, i) => {
                                    return (
                                        <Select.Item
                                            index={i}
                                            key={item.value}
                                            value={item.value}
                                        >
                                            <Select.ItemText>
                                                {item.name}
                                            </Select.ItemText>
                                            <Select.ItemIndicator marginLeft="auto">
                                                <Check size={16} />
                                            </Select.ItemIndicator>
                                        </Select.Item>
                                    );
                                }),
                            [items]
                        )}
                    </Select.Group>
                    {/* Native gets an extra icon */}
                    {props.native && (
                        <YStack
                            position="absolute"
                            right={0}
                            top={0}
                            bottom={0}
                            alignItems="center"
                            justifyContent="center"
                            width={"$4"}
                            pointerEvents="none"
                        >
                            <ChevronDown
                                size={getFontSize(
                                    (props.size as FontSizeTokens) ?? "$true"
                                )}
                            />
                        </YStack>
                    )}
                </Select.Viewport>

                <Select.ScrollDownButton
                    alignItems="center"
                    justifyContent="center"
                    position="relative"
                    width="100%"
                    height="$3"
                >
                    <YStack zIndex={10}>
                        <ChevronDown size={20} />
                    </YStack>
                </Select.ScrollDownButton>
            </Select.Content>
        </Select>
    );
}
