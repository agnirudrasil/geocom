import { useMemo } from "react";
import {
    computeBasePermissions,
    computePermissions,
} from "../src/compute-permissions";
import { rolesSort } from "../src/sort-roles";
import { getUser } from "../src/user-cache";
import { useChannelsStore } from "../stores/useChannelsStore";
import { useGuildsStore } from "../stores/useGuildsStore";
import { Roles, useRolesStore } from "../stores/useRolesStore";

export const usePermssions = (guildId: string, channelId: string) => {
    const roles = useRolesStore(state => state[guildId]);
    const guild = useGuildsStore(state => state.guilds[guildId]) ?? {};
    const guildMember = guild.members?.[getUser()] ?? {};
    const currentChannel =
        useChannelsStore(state => state.channels[guildId]?.[channelId]) ?? {};

    const permissions = useMemo(
        () =>
            computePermissions(
                roles,
                guild,
                guildMember,
                currentChannel as any
            ),
        [roles, guild, guildMember, currentChannel]
    );

    const memberRoles = useMemo(() => {
        if (guildMember) {
            const myRoles = guildMember.roles?.map(r =>
                roles.find(role => role.id === r)
            ) as Roles[];

            myRoles?.sort(rolesSort);

            return myRoles;
        }
        return [];
    }, [roles, guildMember]);

    return {
        permissions,
        roles,
        guild,
        guildMember,
        currentChannel,
        memberRoles,
    };
};

export const useBasePermissions = (guildId: string) => {
    const roles = useRolesStore(state => state[guildId]);
    const guild = useGuildsStore(state => state.guilds[guildId]) ?? {};
    const guildMember = guild.members?.[getUser()] ?? {};

    const permissions = useMemo(
        () => computeBasePermissions(roles, guild, guildMember),
        [roles, guild, guildMember]
    );

    const memberRoles = useMemo(() => {
        if (guildMember) {
            const myRoles = guildMember.roles?.map(r =>
                roles.find(role => role.id === r)
            ) as Roles[];

            myRoles?.sort(rolesSort);

            return myRoles;
        }
        return [];
    }, [roles, guildMember]);

    return {
        permissions,
        roles,
        guild,
        guildMember,
        memberRoles,
    };
};
