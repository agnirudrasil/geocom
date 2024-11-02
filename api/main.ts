import { Application } from "jsr:@oak/oak/application";
import { Router } from "jsr:@oak/oak/router";
import { Client } from "https://deno.land/x/postgres/mod.ts";
import type { MainModule } from "./pathfinding.d.ts";
import { createRequire } from "node:module";
const require = createRequire(import.meta.url);
const MainModuleFactory = require("./pathfinding.js");

const { API, node }: MainModule = await MainModuleFactory();

const text = await Deno.readTextFile("example.json");

const graph = API.initialize_graph(text);

const client = new Client({
    user: "agnirudrasil",
    database: "geocom",
    hostname: "localhost",
    port: 5432,
    password: "",
});
await client.connect();

const router = new Router();
router.get("/pois", async ctx => {
    const q = ctx.request.url.searchParams.get("q");
    const { rows } = await client.queryObject<{ id: string; name: string }>`
        SELECT id::text, name 
        FROM pois 
        WHERE plainto_tsquery('simple', ${q}) @@ document
        `;
    ctx.response.body = rows;
});

router.get("/pois/:id", async ctx => {
    const { id } = ctx.params;
    const { rows } = await client.queryObject<{
        id: string;
        name: string;
        description: string;
        rrule: string;
        pocs: { name: string; email?: string; phone?: string }[];
        coords: number[];
    }>`
        SELECT id::text, name, description, rrule, to_jsonb(pocs) as pocs, to_jsonb(coords) as coords
        FROM pois
        WHERE id = ${id}
        `;
    ctx.response.body = rows[0];
});

router.get("/path", async ctx => {
    const fromId = ctx.request.url.searchParams.get("from");
    const toId = ctx.request.url.searchParams.get("to");
    const { rows } = await client.queryObject<{ closest_coords: number[] }>`
        SELECT to_jsonb(closest_coords) closest_coords
        FROM pois
        WHERE id = ${fromId}
        `;
    const from = rows[0]?.closest_coords;
    const { rows: rowsTo } = await client.queryObject<{
        closest_coords: number[];
    }>`
        SELECT to_jsonb(closest_coords) closest_coords
        FROM pois
        WHERE id = ${toId}
        `;
    const to = rowsTo[0]?.closest_coords;
    const td = new TextDecoder();
    const p = await new Deno.Command("./pathfinding", {
        args: [
            "./example.json",
            ...from.map(s => s.toString()),
            ...to.map(s => s.toString()),
        ],
    }).output();
    console.log(from, to);
    const start = new node(from[0], from[1]);
    const end = new node(to[0], to[1]);
    const _path = API.find_path(graph, start, end);
    const out = td.decode(p.stdout).trim();
    const err = td.decode(p.stderr).trim();
    if (err) {
        console.error("Error in pathfinding", err);
        ctx.response.status = 500;
        ctx.response.body = { error: err };
    }
    const path = JSON.parse(out);
    ctx.response.body = { ...path, from, to };
});

const app = new Application();
app.use(router.routes());
app.use(router.allowedMethods());

app.listen({ port: 8080 });
