const fs=require("fs");
const data = require("./nitk.geo.json"); // Load the GeoJSON data
// Log the imported data to check the structure

const arr = [];

function f(features) {
    for (const feature of features) {
        // Check if the geometry type is "Point"
        if (feature.geometry?.type === "Point") {
            // Check if the "building" tag exists and is "yes"
            if (feature.properties?.tags?.name) {
                arr.push(feature);
            }
        }
    }
    //console.log(arr); // Output the array of matching features
}

// Call the function with the features array
f(data.features); // Pass the features array to the function
const a=JSON.stringify(arr)
fs.writeFile("filtered.json",a,(err)=>{
    if(err){ console.log("Error :/")}
    else{
        console.log(":)")
    }
})
//console.log(data.features.filter((f) => f.geometry?.type === 'Point' && f.properties?.tags?.name).map((f) => f.geometry.coordinates.toReversed()))