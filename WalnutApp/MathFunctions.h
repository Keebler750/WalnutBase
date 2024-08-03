#pragma once

#include <cmath>
#include <vector>
#include <functional>
#include <numeric>
#include <algorithm>

#include <chrono>
#include <thread>

// VARS for DEBUG LOOP CHECKS:
static int ClampFloat1_testMarker = 0;
static int ClampFloat2_testMarker = 0;
static int Haversine_testMarker = 0;
static int DrawEntry_testMarker = 0;
static int WhileLoop_testMarker = 0;

enum setUnits
{
    KM = 1, NM = 2
};

static float conversion = 0.0f;

const float convert_KM = 1.8520000f;
const float convert_NM = 1.0000000f; //NM is our default, and we compare all other distance untis to this.0.5399568f;
const float conver_MILES = 0.6213712f;

constexpr float M_PI = 3.14159265358979323846f;

static float fuelFlow = 15.0f;
static float CorrectedFuelRate = 0.0f;

static int totalFuelUsed = 0;
static float totalDistance = 0.0f;
static int  fuelRemaining = 0;

static float startFuel = 0;

static float fontSize = 0.7f;
static float defaultSize = 70.0f;
static float scaledElementSize = 70.0f;

static int units = 2;   // Sets Units default ENUM to nautical miles
static bool b_debug = false;
static int delay = 40;
static bool b_allowIdling = true;

// switch used to control emplace_back of new waypoint in // MathFunctions.h //
static bool b_IsNewWaypoint = false;
static int WaypointCounter = 0;

static bool b_valueChanged = false;

// STRUCT data type combining all of the waypoint floats.
struct WPT
{
    float LAT_d, LAT_m, LAT_s; // Lat 1
    float LON_d, LON_m, LON_s; // Long 1

    int PROFILE;
    float DISTANCE;
    int FUEL;
    int NS, EW;

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

// STORE ALL WAYPOINT DATA IN VECTOR, if this isn't external and static it won't save properly due to...... while-loop render?
static std::vector<WPT> WaypointEntry;

// Modular creation and storage of waypoint items instead of one big static display of onscreen entries.
class WAYPOINT
{
public:
    float lat_d, lat_m, lat_s; // Latitude of each wptID waypoint, placed into vector
    float lon_d, lon_m, lon_s; // Longitude of each wptID waypoint, placed into vector

    int profile;
    float distance;
    int fuel;

    int ns, ew;

    // Default constructor
    WAYPOINT()
        : lat_d(0.0f), lat_m(0.0f), lat_s(0.0f), lon_d(0.0f), 
        lon_m(0.0f), lon_s(0.0f), profile(0), distance(0.0f), fuel(0), ns(0), ew(0)
    {

    }

    // FUNCTION - pass in var which represents waypoint number FOR NOW. Helps with screen element IDs as well, for the hash.
    void drawEntry(int m_vectorPOS, int units)
    {
        if (b_IsNewWaypoint) // This check mechanism allows us to only push back an element ONCE when creating a new waypoint, to avoid a race condition.
        {
            // create a spot in the vector, and initializes with zero for the input box.
            WaypointEntry.emplace_back(lat_d, lat_m, lat_s, lon_d, lon_m, lon_s, profile, distance, fuel, ns, ew);
            
            b_IsNewWaypoint = false; // reset until value is made true again by button press to create waypoint
        }

        DrawEntry_testMarker++; //used to test the loop incrementation. Output is on WalnutApp.cpp near the "+" button

        // SIZE TEST to make sure vector is not growing due to loop:
        // ImGui::Text("Entry size: %d", Entry.size());
        ImGui::PushItemWidth(scaledElementSize); // sets input box width until the POP below.

            ImGui::Text("Waypoint : %d", m_vectorPOS);

            //This differentiates the ImGui data labels through each loop
            // to avoid dupes because it is a HASH of PushID + DataLabel
            ImGui::PushID(m_vectorPOS);     

                    //INPUT AND DISPLAY WPT LAT, including NORTH/SOUTH:
                    ImGui::Text("LAT   ");

            ImGui::SameLine();

                ImGui::PushItemWidth(scaledElementSize);/// width for combo button

                    if (ImGui::Combo("##1", &WaypointEntry.at(m_vectorPOS).NS, "North\0South\0", 2))
                    {
                        WaypointEntry.at(m_vectorPOS).NS = WaypointEntry.at(m_vectorPOS).NS;
                        b_valueChanged = true;
                    }

                ImGui::SameLine();

                ImGui::PopItemWidth(); ImGui::SameLine();
                //testMarker++; //used to test the loop incrementation. Output is on WalnutApp.cpp near the "+" button

                    // INPUT LATITUDE:
                    // NULL takes away the increment/decrement range creation button
                    // Clamp values: 90 Latitude degrees
                    ClampInputFloat("DEG    ##LAT1", &WaypointEntry.at(m_vectorPOS).LAT_d, NULL, NULL, "%.5f", 0.0f, 90.0f, NULL); ImGui::SameLine();
                    ClampInputFloat("MIN    ##LAT1", &WaypointEntry.at(m_vectorPOS).LAT_m, NULL, NULL, "%.5f", 0.0f, 60.0f, NULL); ImGui::SameLine();
                    ClampInputFloat("SEC    ##LAT1", &WaypointEntry.at(m_vectorPOS).LAT_s, NULL, NULL, "%.5f", 0.0f, 60.0f, NULL);

                    // COLUMN 2: on the second waypoint, add column two input and display: Flight profile
                    if (m_vectorPOS >= 1)
                    {
                            ImGui::SameLine(); ImGui::Dummy(ImVec2(50.0f, 0.0f)); ImGui::SameLine();

                            ImGui::InputInt("  Flight Profile", &WaypointEntry.at(m_vectorPOS).PROFILE, NULL, NULL);
                    }

                    //INPUT AND DISPLAY WPT LONG, including EAST/WEST:
                    ImGui::Text("LONG"); ImGui::SameLine();

                ImGui::PushItemWidth(scaledElementSize);/// width for combo button

                    //Set East/West:
                    if (ImGui::Combo("##2", &WaypointEntry.at(m_vectorPOS).EW, "East\0West\0", 2))
                    {
                        WaypointEntry.at(m_vectorPOS).EW = WaypointEntry.at(m_vectorPOS).EW;
                        b_valueChanged = true;
                    }

                ImGui::SameLine(); ImGui::PopItemWidth(); ImGui::SameLine();

                    // INPUT LONGITUDE:
                    // NULL takes away the increment/decrement range creation buttons
                    // Clamp values, 180 Longitude degrees
                    ClampInputFloat("DEG    ##LON1", &WaypointEntry.at(m_vectorPOS).LON_d, NULL, NULL, "%.5f", 0.0f, 180.0f, NULL); ImGui::SameLine();
                    ClampInputFloat("MIN    ##LON1", &WaypointEntry.at(m_vectorPOS).LON_m, NULL, NULL, "%.5f", 0.0f, 60.0f, NULL); ImGui::SameLine();
                    ClampInputFloat("SEC    ##LON1", &WaypointEntry.at(m_vectorPOS).LON_s, NULL, NULL, "%.5f", 0.0f, 60.0f, NULL);

                    char* unitString = "test";

                    // COLUMN 2: INPUT AND DISPLAY: Leg distance and fuel used
                    // Don't calc (OR SHOW!) a distance if we only have one waypoint
                    // ---> Calculate the distance and store the data in the vector
                    if (m_vectorPOS >= 1)
                    {
                        ImGui::SameLine(); ImGui::Dummy(ImVec2(50.0f, 0.0f)); ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.7f, 0.3f));

                            // Re-run when any of the fuel, unit or distance data have changed.
                            if (b_valueChanged)
                            {
                                runDistanceCalc(m_vectorPOS);
                            }

                            // Set the text label:
                            if (units == NM)
                                unitString = "NM";
                            if (units == KM)
                                unitString = "KM";

                            WaypointEntry.at(m_vectorPOS).FUEL = (int)(CorrectedFuelRate * WaypointEntry.at(m_vectorPOS).DISTANCE);
                            ImGui::InputFloat("##LegDist", &WaypointEntry.at(m_vectorPOS).DISTANCE, NULL, NULL, "%.0f"); ImGui::SameLine(); ImGui::Text("Leg Length (%s)", unitString); // <<<<<<<<
                            ImGui::SameLine(); ImGui::Dummy(ImVec2(30.0f, 0.0f)); ImGui::SameLine();
                            ImGui::InputInt("Approx. Fuel Used (LBS)", &WaypointEntry.at(m_vectorPOS).FUEL, NULL, NULL);

                        ImGui::PopStyleColor();
                    }

            ImGui::PopID();

            ImGui::Dummy(ImVec2(0.0f, 10.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::PopItemWidth();
    }

    void totalFuelCalc()
    {
        totalFuelUsed = std::accumulate(WaypointEntry.begin(), WaypointEntry.end(), 0,
        [](int sum, const WPT& e) { return sum + e.FUEL; });    // Lambda here....no idea exactly how it works!!!
    }

    void totalDistanceCalc()
    {
        totalDistance = std::accumulate(WaypointEntry.begin(), WaypointEntry.end(), 0.0f,
        [](float sum, const WPT& e) { return sum + e.DISTANCE; });    // Lambda here....no idea exactly how it works!!!
    }

    void fuelRemainingCalc()
    {
        fuelRemaining = (int)startFuel - totalFuelUsed;
    }

    // Wrapper for ImGui::InputFloat to add clamp, includes an activation counter and CHANGED flag
    // value_min and value_max added in wrapper.
    // b_valueChanged is set and read through this page and WalnutApp.cpp to re-calc only when necessary.
    void ClampInputFloat(const char* label, float* v, float step, float step_fast, const char* format, float value_min, float value_max, ImGuiInputTextFlags flags)
    {
        ClampFloat1_testMarker++;

        if (ImGui::InputFloat(label, v, step, step_fast, format, flags))
        {
            *v = std::clamp(*v, value_min, value_max);

            b_valueChanged = true;

            ClampFloat2_testMarker++;
        }
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
            float signEW = -1.0f;
            return signEW;
        }

        else
        {
            return 0;
        }

    }

    // C++ code for the haversine formula, which outputs in NAUTICAL MILES
    // Core math for distance calc from position data. Requires decimal degrees to four place precision.
    float haversine(int position/*, float lat1, float lon1, float lat2, float lon2*/)
    {
        // init
        float lat1{ 0.0f }, lat2{ 0.0f }, lon1{ 0.0f }, lon2{ 0.0f };

        if (position >= 1)
        {   // Calculate the decimal degree value of the LAT/LONG data for use with the rest of the haversine
            lat1 = (WaypointEntry.at(position - 1).LAT_d + (WaypointEntry.at(position - 1).LAT_m / 60) + (WaypointEntry.at(position - 1).LAT_s / 3600));
            lon1 = (WaypointEntry.at(position - 1).LON_d + (WaypointEntry.at(position - 1).LON_m / 60) + (WaypointEntry.at(position - 1).LON_s / 3600));
            lat2 = (WaypointEntry.at(position).LAT_d + (WaypointEntry.at(position).LAT_m / 60) + (WaypointEntry.at(position).LAT_s / 3600));
            lon2 = (WaypointEntry.at(position).LON_d + (WaypointEntry.at(position).LON_m / 60) + (WaypointEntry.at(position).LON_s / 3600));
        }


        Haversine_testMarker++; // place or turn this on to test loop count

        // distance between latitudes and longitudes
        float deltaLAT = ((lat2 * (signNorthSouth(WaypointEntry.at(position).NS))) - (lat1 * (signNorthSouth(WaypointEntry.at(position - 1).NS)))) *
            M_PI / 180.0f;
        float deltaLON = ((lon2 * (signEastWest(WaypointEntry.at(position).EW))) - (lon1 * (signEastWest(WaypointEntry.at(position - 1).EW)))) *
            M_PI / 180.0f;

        // convert to radians
        lat1 = (lat1)*M_PI / 180.0f;
        lat2 = (lat2)*M_PI / 180.0f;

        // apply formulae
        float a = pow(sin(deltaLAT / 2.0f), 2.0f) +
            pow(sin(deltaLON / 2.0f), 2.0f) *
            cos(lat1) * cos(lat2);
        float rad = 6371.0f;
        float c = 2.0f * asin(sqrt(a));
        return (0.5399568f) * rad * c; // Output is in NAUTICAL MILES (correction factor listed is from KM to NM).
    }

    void runDistanceCalc(int position)
    {
        if (units == NM)
            conversion = convert_NM;

        if (units == KM)
            conversion = convert_KM;

        CorrectedFuelRate = (1 / conversion) * fuelFlow;

        WaypointEntry.at(position).DISTANCE = conversion * haversine(position);

        b_valueChanged = false;
    }

};



/* 
// Driver code
//
// Portions of haversine formula code contributed by Mahadev.
//
*/