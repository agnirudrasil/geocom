import { ListItem } from "tamagui";
import { Link } from "expo-router";
import { SearchResults } from "@/components/search-results";

export default function TabOneScreen() {
    return (
        <SearchResults>
            {poi => (
                <Link key={poi.id} href={`/modal?poi=${poi.id}`}>
                    <ListItem title={poi.name} />
                </Link>
            )}
        </SearchResults>
    );
}
