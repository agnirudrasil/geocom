import {
    Clear,
    KeyboardBackspace,
    Search,
    TrendingUp,
} from "@mui/icons-material";
import {
    Box,
    Button,
    Card,
    CardContent,
    CardHeader,
    Grid,
    IconButton,
    ImageList,
    ImageListItem,
    InputAdornment,
    Paper,
    TextField,
    Typography,
} from "@mui/material";
import { ReactNode, useEffect, useRef, useState } from "react";
import { Waypoint } from "react-waypoint";
import { useQuery } from "react-query";
import { request } from "../../request";
import { useGifsSuggest } from "../../../hooks/requests/useGifsSuggest";
import { useGifsShare } from "../../../hooks/requests/useGifsShare";
import { useDebounce } from "use-debounce";

const Item: React.FC<{
    isLoading: boolean;
    src?: string;
    name?: string | ReactNode;
    color?: string;
    dims?: number[];
}> = ({ src, name, color, dims }) => {
    return (
        <Paper
            sx={theme => ({
                width: 195,
                height: dims ? `${195 / (dims[0] / dims[1])}` : 110,
                position: "relative",
                transition: "border 200ms ease",
                boxSizing: "border-box",
                "&:hover": {
                    border: "3px solid #00b176",
                },
                borderRadius: theme.shape.borderRadius,
                cursor: "pointer",
                overflow: "hidden",
            })}
            variant="outlined"
        >
            {src && (
                <img
                    src={src}
                    alt={name && typeof name === "string" ? name : src}
                    width="195px"
                    height={dims ? `${195 / (dims[0] / dims[1])}px` : "110px"}
                    style={{
                        objectFit: "cover",
                        zIndex: 5,
                    }}
                />
            )}
            {name && (
                <Box
                    style={{
                        position: "absolute",
                        top: 0,
                        left: 0,
                        height: "100%",
                        width: "100%",
                        zIndex: 7,
                        display: "flex",
                        justifyContent: "center",
                        alignItems: "center",
                        background: color ?? "rgba(0, 0, 0, 0.3)",
                    }}
                >
                    <Typography
                        sx={{
                            textShadow: "1px 1px rgba(0, 0, 0, 0.5)",
                            fontWeight: "bold",
                            display: "flex",
                            alignItems: "center",
                            justifyContent: "center",
                            gap: "0.5rem",
                        }}
                        variant="subtitle1"
                    >
                        {name}
                    </Typography>
                </Box>
            )}
        </Paper>
    );
};

const fetchFn = async (key: any) => {
    if (key.queryKey[1]) {
        const data = await request(
            `${process.env.NEXT_PUBLIC_API_URL}/gifs/search?q=${key.queryKey[1]}`
        );

        return data.json();
    }

    if (key.queryKey[2]) {
        const data = await request(
            `${process.env.NEXT_PUBLIC_API_URL}/gifs/trending`
        );

        return data.json();
    }

    const data = await request(
        `${process.env.NEXT_PUBLIC_API_URL}/gifs/categories`
    );

    return data.json();
};

export const GifPicker: React.FC<{
    onShare: (stuff: {
        url: string;
        src: string;
        width: number;
        height: number;
    }) => any;
}> = ({ onShare }) => {
    const [search, setSearch] = useState("");
    const [view, setView] = useState("");
    const { mutateAsync } = useGifsShare();
    const [debouncedSearch] = useDebounce(search, 500);
    const { data, isLoading } = useQuery(
        ["gifs", debouncedSearch, view],
        fetchFn,
        {
            cacheTime: Infinity,
            staleTime: Infinity,
        }
    );
    const { data: suggest, refetch } = useGifsSuggest(search);
    const ref = useRef<HTMLDivElement | null>(null);

    useEffect(() => {
        if (ref.current) {
            ref.current.scrollTo(0, 0);
        }
    }, [search]);

    return (
        <Card
            sx={{
                flexGrow: 1,
                maxWidth: "450px",
            }}
        >
            <CardHeader
                sx={{
                    position: "sticky",
                    top: 0,
                    left: 0,
                    zIndex: 5,
                    bgcolor: "background.default",
                    boxShadow: `0 1px 0 0 rgb(24 25 28 / 30%), 0 1px 2px 0 rgb(24 25 28 / 30%)`,
                }}
                title={
                    <Typography
                        variant="subtitle1"
                        sx={{ marginBottom: "0.5rem" }}
                    >
                        GIFs
                    </Typography>
                }
                subheader={
                    view ? (
                        <div>
                            <IconButton onClick={() => setView("")}>
                                <KeyboardBackspace />
                            </IconButton>
                            <Typography variant="button">
                                Trending Gifs
                            </Typography>
                        </div>
                    ) : (
                        <TextField
                            placeholder="Search Tenor"
                            fullWidth
                            size="small"
                            variant="outlined"
                            value={search}
                            onChange={e => setSearch(e.target.value)}
                            InputProps={{
                                endAdornment: search ? (
                                    <IconButton
                                        onClick={() => setSearch("")}
                                        size="small"
                                    >
                                        <Clear />
                                    </IconButton>
                                ) : (
                                    <InputAdornment position="end">
                                        <Search />
                                    </InputAdornment>
                                ),
                            }}
                        />
                    )
                }
            />
            <CardContent
                ref={ref}
                sx={{
                    maxHeight: "400px",
                    overflow: "auto",
                }}
            >
                <ImageList variant="masonry" cols={2}>
                    {!search && !view && (
                        <ImageListItem onClick={() => setView("trend")}>
                            <Item
                                name={
                                    <>
                                        <TrendingUp /> Trending
                                    </>
                                }
                                color="#00b176"
                                isLoading={isLoading}
                            />
                        </ImageListItem>
                    )}
                    {(data || Array(50).fill({ src: "", name: "" })).map(
                        (i: any, index: number) => (
                            <ImageListItem
                                key={index}
                                onClick={async () => {
                                    if (search || view) {
                                        await mutateAsync({
                                            search,
                                            id: i?.id || "",
                                        });
                                        await onShare({
                                            url: i?.url || "",
                                            src: i?.gif_src || "",
                                            width: i.width,
                                            height: i.height,
                                        });
                                    } else {
                                        setSearch(i?.name);
                                    }
                                }}
                            >
                                <Item
                                    key={i.id}
                                    src={search || view ? i?.gif_src : i?.src}
                                    name={i?.name}
                                    isLoading={isLoading}
                                    dims={
                                        search || view
                                            ? [i?.width, i?.height]
                                            : undefined
                                    }
                                />
                            </ImageListItem>
                        )
                    )}
                </ImageList>
                <Waypoint
                    onEnter={() => {
                        if (search) {
                            refetch();
                        }
                    }}
                />
                {suggest && (
                    <Grid container spacing={3}>
                        {suggest.map((s: string) => (
                            <Grid key={s} item xs="auto">
                                <Button
                                    onClick={() => setSearch(s)}
                                    variant="outlined"
                                    size="small"
                                >
                                    {s}
                                </Button>
                            </Grid>
                        ))}
                    </Grid>
                )}
            </CardContent>
        </Card>
    );
};
