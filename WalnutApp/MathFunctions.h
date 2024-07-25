#pragma once

#include <cmath>
#include <vector>
#include <functional>
#include <numeric>
#include <algorithm>

enum class Aircraft {
    None, F18, F15, A10
};

Aircraft AircraftType = Aircraft::None;

static int testMarker = 0;

constexpr float M_PI = 3.14159265358979323846f;

static int totalFuelUsed = 0;
static float totalDistance = 0.0f;
static int  fuelRemaining = 0;

static float startFuel = 0;

static float fontSize = 0.8f;
static float defaultSize = 70;
static float scaledElementSize = 70.0f;

static int units = 2;   // Sets Units default ENUM to nautical miles


    // STRUCT data type combining all of the waypoint floats.
struct WPT
{
    float LAT_d; // Lat 1
    float LAT_m;
    float LAT_s;

    float LON_d; // Long 1
    float LON_m;
    float LON_s;

    int PROFILE;
    float DISTANCE;
    int FUEL;
    int NS;
    int EW;

        // Default Constructor with initialization
    WPT()
        :LAT_d(0.0f), LAT_m(0.0f), LAT_s(0.0f), LON_d(0.0f), LON_m(0.0f), LON_s(0.0f), PROFILE(0), DISTANCE(0.0f), FUEL(0), NS(0), EW(0)
    {

    }

        // Constructor, initialized by parameters passed in.
    WPT(float a, float b, float c, float d, float e, float f, int g, float h, int i, int j, int k)
        :LAT_d(a), LAT_m(b), LAT_s(c), LON_d(d), LON_m(e), LON_s(f), PROFILE(g), DISTANCE(h), FUEL(i), NS(j), EW(k)
    {

    }
};


    // if this isn't external and static it won't save properly due to...... while-loop render?
static std::vector<WPT> Entry;


    // Modular creation and storage of waypoint items instead of one big static display of onscreen entries.
class WAYPOINT
{
public:
    // These variables will be stored in vector; all others are transient or recalculated.
    float lat_d; // Latitude of each wptID waypoint, placed into vector
    float lat_m;
    float lat_s;

    float lon_d; // Longitude of each wptID waypoint, placed into vector
    float lon_m;
    float lon_s;

    int profile;
    float distance;
    int fuel;

    int ns;
    int ew;

        // Default constructor
    WAYPOINT()
        //: lat_d(0.0f), lat_m(0.0f), lat_s(0.0f), lon_d(0.0f), 
        //lon_m(0.0f), lon_s(0.0f), profile(0), distance(0.0f), fuel(0), ns(0), ew(0)
    {

    }

        // FUNCTION - pass in var which represents waypoint number. Helps with screen element IDs as well, for the hash.
    void drawEntry(int a, int units, bool& b_IsNewWPT)
    {
        if (b_IsNewWPT) // This check mechanism allows us to only push back an element ONCE when creating a new waypoint, to avoid a race condition.
        {
            // creates, and initializes with zero for the input box.
            Entry.emplace_back(lat_d, lat_m, lat_s, lon_d, lon_m, lon_s, profile, distance, fuel, ns, ew);
            testMarker++;
            b_IsNewWPT = false;
            //b_IsNewWaypoint = false; // This is the proper way to do it, I think. Why pass bool value in? Just ref it or use directly
        }

        // SIZE TEST:
        // ImGui::Text("Entry size: %d", Entry.size());
        ImGui::PushItemWidth(scaledElementSize); // sets input box width until the POP below.

            ImGui::Text("Waypoint : %d", a); //

            ImGui::PushID(a);

            //WPT LAT:
            ImGui::Text("LAT   ");
            ImGui::SameLine();

                ImGui::PushItemWidth(scaledElementSize);/// width for combo button

                    if (ImGui::Combo("##1", &Entry.at(a).NS, "North\0South\0", 2))
                    {
                        Entry.at(a).NS = Entry.at(a).NS;
                    }
                    ImGui::SameLine();

                ImGui::PopItemWidth(); ImGui::SameLine();

            ImGui::InputFloat("DEG    ##LAT1", &Entry.at(a).LAT_d, NULL, NULL, "%.5f");// NULL takes away the increment/decrement range creation button
            Entry.at(a).LAT_d = (std::clamp(Entry.at(a).LAT_d, 0.0f, 90.0f)); // max 90 degreees of Latitude. 

            ImGui::SameLine();
            ImGui::InputFloat("MIN    ##LAT1", &Entry.at(a).LAT_m, NULL, NULL, "%.5f");
            Entry.at(a).LAT_m = std::clamp(Entry.at(a).LAT_m, 0.0f, 60.0f); // max 60 minutes in a degree

            ImGui::SameLine();
            ImGui::InputFloat("SEC    ##LAT1", &Entry.at(a).LAT_s, NULL, NULL, "%.5f");
            Entry.at(a).LAT_s = std::clamp(Entry.at(a).LAT_s, 0.0f, 60.0f); // max 60 seconds in a minute

                // column two info:
            if (a >= 1)
            {
                ImGui::SameLine(); ImGui::Dummy(ImVec2(50.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputInt("  Flight Profile", &Entry.at(a).PROFILE, NULL, NULL);
            }

                //WPT LONG:
            ImGui::Text("LONG");
            ImGui::SameLine();

                ImGui::PushItemWidth(scaledElementSize);/// width for combo button

                    if (ImGui::Combo("##2", &Entry.at(a).EW, "East\0West\0", 2))
                    {
                        Entry.at(a).EW = Entry.at(a).EW;
                    }
            
                    ImGui::SameLine();

                ImGui::PopItemWidth(); ImGui::SameLine();

            ImGui::InputFloat("DEG    ##LON1", &Entry.at(a).LON_d, NULL, NULL, "%.5f"); // NULL takes away the increment/decrement range creation buttons
            Entry.at(a).LON_d = std::clamp(Entry.at(a).LON_d, 0.0f, 180.0f);    // Max of 180 degrees of Longitude

            ImGui::SameLine();
            ImGui::InputFloat("MIN    ##LON1", &Entry.at(a).LON_m, NULL, NULL, "%.5f");
            Entry.at(a).LON_m = std::clamp(Entry.at(a).LON_m, 0.0f, 60.0f);

            ImGui::SameLine();
            ImGui::InputFloat("SEC    ##LON1", &Entry.at(a).LON_s, NULL, NULL, "%.5f");
            Entry.at(a).LON_s = std::clamp(Entry.at(a).LON_s, 0.0f, 60.0f);

                // Don't calc a distance if we only have one waypoint
                // ---> Calculate the distance and store the data in the vector
            if (a >= 1)
            { 
                if (units == 1)         // KM
                {
                Entry.at(a).DISTANCE = haversine(a /*position*/, (Entry.at(a - 1).LAT_d + (Entry.at(a - 1).LAT_m / 60) + (Entry.at(a - 1).LAT_s / 3600)),    //Lat1
                                                    (Entry.at(a - 1).LON_d + (Entry.at(a - 1).LON_m / 60) + (Entry.at(a - 1).LON_s / 3600)),    //Long1
                                                    (Entry.at(a).LAT_d + (Entry.at(a).LAT_m / 60) + (Entry.at(a).LAT_s / 3600)),    //Lat2
                                                    (Entry.at(a).LON_d + (Entry.at(a).LON_m / 60) + (Entry.at(a).LON_s / 3600)));   //Long2
                }
                else if (units == 2)    // NM
                {
                Entry.at(a).DISTANCE =  ConvertDistance (
                                        haversine(a /*position*/, (Entry.at(a - 1).LAT_d + (Entry.at(a - 1).LAT_m / 60) + (Entry.at(a - 1).LAT_s / 3600)),   //Lat1
                                                    (Entry.at(a - 1).LON_d + (Entry.at(a - 1).LON_m / 60) + (Entry.at(a - 1).LON_s / 3600)),    //Long1
                                                    (Entry.at(a).LAT_d + (Entry.at(a).LAT_m / 60) + (Entry.at(a).LAT_s / 3600)),    //Lat2
                                                    (Entry.at(a).LON_d + (Entry.at(a).LON_m / 60) + (Entry.at(a).LON_s / 3600)))   //Long2
                                                        );
                }
                else
                {

                }
            }

            else    // If only one waypoint: 
            {
                Entry.at(a).DISTANCE = 0.0;
            }

                // column two info:
            if (a >= 1) 
            {
                ImGui::SameLine(); ImGui::Dummy(ImVec2(50.0f, 0.0f)); ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.7f, 0.3f));

                if (units == 1)
                {

                    int value = 15;
                    Entry.at(a).FUEL = (int)(value * Entry.at(a).DISTANCE);
                    ImGui::InputFloat("  Leg (KM)        ", &Entry.at(a).DISTANCE, NULL, NULL, "%.3f");
                    ImGui::SameLine();
                    ImGui::InputInt("  Approx. Fuel Used (LBS)", &Entry.at(a).FUEL, NULL, NULL);

                }
                else
                {

                    int value = 15;  
                    Entry.at(a).FUEL = (int)(value * Entry.at(a).DISTANCE);
                    ImGui::InputFloat("  Leg (NM)        ", &Entry.at(a).DISTANCE, NULL, NULL, "%.1f");
                    ImGui::SameLine();
                    ImGui::InputInt("  Approx. Fuel Used (LBS)", &Entry.at(a).FUEL, NULL, NULL);

                }

                ImGui::PopStyleColor();
            }
            ImGui::PopID();

                // Component that allows for format/spacing that is invisible
            ImGui::Dummy(ImVec2(0.0f, 10.0f)); 
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 10.0f));


        ImGui::PopItemWidth();

    }

        // Get the sign for North/South latitude to make the formula work anywhere in the world.
    static float signNorthSouth(int a)
    {
        if (a == 0)
        {
            float signNS = 1.0f;
            return signNS;
        }
        else if (a == 1)
        {
            float signNS = -1.0f;
            return signNS;
        }
        else
        {
            return 0;
        }

    }

        // Get the sign for East/West longitude to make the formula work anywhere in the world.
    static float signEastWest(int a)
    {
        if (a == 0)
        {
            float signEW = 1.0f;
            return signEW;
        }
        else if (a == 1)
        {
            float signEW = - 1.0f;
            return signEW;
        }

        else
        {
            return 0;
        }
        
    }

        // C++ code for the haversine formula
        // Core math for distance calc from position data. Requires decimal degrees to four place precision.
    static float haversine(int position, float lat1, float lon1, float lat2, float lon2 )
{
    //int position = 0; // TEST, crashes due to vector pos.
    
        

        //testMarker++; // place or turn this on to test loop count

        // distance between latitudes and longitudes
    float dLat = ((lat2 * (signNorthSouth(Entry.at(position).NS))) - (lat1 * (signNorthSouth(Entry.at(position - 1).NS)))) *
        M_PI / 180.0f;
    float dLon = ((lon2 * (signEastWest(Entry.at(position).EW))) - (lon1 * (signEastWest(Entry.at(position - 1).EW)))) *
        M_PI / 180.0f;

// convert to radians
    lat1 = (lat1)*M_PI / 180.0f;
    lat2 = (lat2)*M_PI / 180.0f;

    // apply formulae
    float a = pow(sin(dLat / 2.0f), 2.0f) +
        pow(sin(dLon / 2.0f), 2.0f) *
        cos(lat1) * cos(lat2);
    float rad = 6371.0f;
    float c = 2.0f * asin(sqrt(a));
    return rad * c; // Calculated distance between the two points.
}

        // Convert km to nm when needed
    float ConvertDistance(float fResult)
    {
        float nResult = 0.0f;
        return  nResult = fResult * 0.5399568f; // to nautical miles
    }

    void totalFuelCalc()
    {
        totalFuelUsed = std::accumulate(Entry.begin(), Entry.end(), 0,
        [](int sum, const WPT& e) { return sum + e.FUEL; });    // Lambda here....no idea exactly how it works!!!
    }

    void totalDistanceCalc()
    {
        totalDistance = std::accumulate(Entry.begin(), Entry.end(), 0.0f,
        [](float sum, const WPT& e) { return sum + e.DISTANCE; });    // Lambda here....no idea exactly how it works!!!
    }

    void fuelRemainingCalc()
    {
        fuelRemaining = (int)startFuel - totalFuelUsed;

    }



    //void drawTotals(float &startFuel, int a )//??/ what do ACTUALLY I need to pass in?
    //{
    //    static float fuelRemain = 0;
    //    static float totalFuelUsed = 0;
    //    static float totalDistance = 0;

    //    totalDistance = calculateTotalDistance();
    //    totalFuelUsed = calculateTotalFuelUsed(a);
    //    fuelRemain = calculateTotalFuelRemain(totalFuelUsed, startFuel);

    //    
    //        ImGui::Dummy(ImVec2(0.0f, 60.0f));
    //    ImGui::PushItemWidth(50);
    //        ImGui::Dummy(ImVec2(200.0f, 0.0f)); ImGui::SameLine();
    //        ImGui::Text("TOTALS"); 
    //        ImGui::Text("Entry.at(x).FUEL is %f here", Entry.at(a).FUEL);
    //        ImGui::Dummy(ImVec2(200.0f, 0.0f)); ImGui::SameLine();
    //    
    //    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.7f, 0.4f));
    //        
    //        ImGui::InputFloat("  Total Distance (units)", &totalDistance, NULL, NULL, "%.0f");
    //        ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
    //        ImGui::InputFloat("  Projected Fuel Use (LBS)", &totalFuelUsed, NULL, NULL, "%.0f");
    //        ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
    //        ImGui::InputFloat("  Projected Fuel Remaining (LBS)", &fuelRemain, NULL, NULL, "%.0f");
    //    
    //    ImGui::PopStyleColor();
    //    ImGui::PopItemWidth();

    //    // Component that allows for format/spacing that is invisible
    //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    //    ImGui::Separator();
    //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    //}

};



/* 
// Driver code
//
// Portions of haversine formula code contributed by Mahadev.
//
//
*/