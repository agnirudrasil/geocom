import produce from "immer";
import create from "zustand";
import { combine } from "zustand/middleware";
import { rolesSort } from "../src/sort-roles";

export interface Roles {
    id: string;
    name: string;
    color: number;
    position: number;
    permissions: string;
    mentionable: boolean;
}

export const useRolesStore = create(
    combine(
        {
            ...({} as Record<string, Roles[]>),
        },
        set => ({
            setRoles: (roles: Record<string, Roles[]>) => set(() => roles),
            addGuild: (guildId: string, roles: Roles[]) =>
                set(state =>
                    produce(state, draft => {
                        draft[guildId] = roles;
                    })
                ),
            deleteGuild: (guildId: string) =>
                set(state =>
                    produce(state, draft => {
                        delete draft[guildId];
                    })
                ),
            addRole: (guildId: string, role: Roles) =>
                set(state =>
                    produce(state, draft => {
                        if (!draft[guildId]) {
                            draft[guildId] = [];
                        }
                        draft[guildId].splice(
                            draft[guildId].length - 1,
                            0,
                            role
                        );
                        draft[guildId].sort(rolesSort).reverse();
                    })
                ),
            deleteRole: (guildId: string, roleId: string) =>
                set(state =>
                    produce(state, draft => {
                        if (draft[guildId]) {
                            draft[guildId] = draft[guildId].filter(
                                r => r.id !== roleId
                            );
                            draft[guildId].sort(rolesSort).reverse();
                        }
                    })
                ),
            updateRole: (guildId: string, role: Roles) =>
                set(state =>
                    produce(state, draft => {
                        if (draft[guildId]) {
                            const index = draft[guildId].findIndex(
                                r => r.id === role.id
                            );
                            draft[guildId][index] = role;
                            draft[guildId].sort(rolesSort).reverse();
                        }
                    })
                ),
        })
    )
);
