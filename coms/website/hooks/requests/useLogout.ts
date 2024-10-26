import Router from "next/router";
import { useMutation } from "react-query";
import { setAccessToken } from "../../src/access-token";
import { request } from "../../src/request";

export const logout = async () => {
    const res = await request(
        `${process.env.NEXT_PUBLIC_API_URL}/auth/logout`,
        {
            method: "POST",
            credentials: "include",
        }
    );

    if (!res.ok) {
        throw new Error("error in logout");
    }

    return "";
};

export const useLogout = () => {
    return useMutation(logout, {
        onSuccess: () => {
            setAccessToken("");
            Router.replace(`/login?next=${Router.asPath}`);
        },
    });
};
