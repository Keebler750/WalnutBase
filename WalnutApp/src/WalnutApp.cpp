#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "../F18Data.h"


/// <NOTES>
/// Walnut/Application.cpp, line 480-ish contains window style setup
/// WAYPOINT class instantiated way over in Entry.h to avoid constant hits from the while-loop
/// </NOTES>


class ExampleLayer : public Walnut::Layer
{
public:
    virtual void OnUIRender() override
    {
        bool show_demo_window = false;
        bool show_ID_StackTool = false;
        bool lockWindow = true; // Enable the child window locking into main window - Go manually set the Begin flags !

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow();

        }

        if (show_ID_StackTool)
        {
            ImGui::ShowStackToolWindow();
        }

        if (lockWindow)
        {
            // Set by bool above, this is a layout item; also needs the WindowFlags re-enabled in "Begin"
            // This section conforms the child to the main window dimensions.            
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);     // These settings, instead of 'Pos' and 'Size'... 
            ImGui::SetNextWindowSize(viewport->WorkSize);   //avoid drawing over main menubar.
        }

        // Create a window inside main window called "Application Window" and append into it.
        // un-comment the windowFlags when the "lockWindow = true" above.
        ImGui::Begin("##MainAppWindowFuelCalc", NULL
            , ImGuiWindowFlags_AlwaysHorizontalScrollbar |
            ImGuiWindowFlags_AlwaysVerticalScrollbar |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove
        );

        // Slow down the loop
        if (b_allowIdling)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        // if, in the last loop, we asked to delete the last sequence point, 
        // then we wait till here to pop it off the vector to avoid errors. The waypoint counter NEEDS 
        // to follow the actions to avoid a draw error in the table of values.
        if (b_requestPopBack)   
        {
            WaypointEntry.pop_back();
            WaypointCounter--;
            b_requestPopBack = false;
        }

        // Menu item to load some default data. Useful for display or debugging.
        if (b_loadData)
        {
            //WaypointEntry.clear();
            WaypointEntry.emplace_back(45, 32, 0, 39, 27, 0, 0, 0, 0, 0, 0, "Home Airfield");
            WaypointEntry.emplace_back(47, 21, 0, 36, 49, 0, 0, 0, 0, 0, 0, "Join up");
            WaypointEntry.emplace_back(42, 41, 0, 32, 18, 0, 0, 0, 0, 0, 0, "Orbit Target");
            b_loadData = false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Combo Boxes are also called "Dropdown" in other systems
        // Expose flags as checkbox for the demo
        static ImGuiComboFlags flags = 0;

        // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* alt_menu[] = { "0 ft","1000 ft", "5000 ft", "10,000ft", "15,000ft", "20,000ft", "25,000ft", "30,000ft", "35,000ft", "40,000ft", "45,000ft" };
        static int alt_current_idx = 0; // Here we store our selection data as an index.

        const char* mach_menu[] = { "0 M", "0.5 M", "0.6 M", "0.7 M", "0.8 M", "0.85 M" };
        static int mach_current_idx = 0;

        // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
        const char* alt_preview_value = alt_menu[alt_current_idx];
        const char* mach_preview_value = mach_menu[mach_current_idx];

        ImGui::PushItemWidth(60);

        if (ImGui::BeginCombo(" ALT##menu", alt_preview_value, ImGuiComboFlags_HeightLargest | ImGuiComboFlags_NoArrowButton))
        {
            for (int n = 0; n < IM_ARRAYSIZE(alt_menu); n++)
            {
                const bool is_selected = (alt_current_idx == n);
                if (ImGui::Selectable(alt_menu[n], is_selected))
                    alt_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }ImGui::EndCombo();

        } ImGui::SameLine();

        if (ImGui::BeginCombo(" MACH##menu",mach_preview_value, ImGuiComboFlags_HeightLargest | ImGuiComboFlags_NoArrowButton))
        {
            for (int n = 0; n < IM_ARRAYSIZE(mach_menu); n++)
            {
                const bool is_selected = (mach_current_idx == n);
                if (ImGui::Selectable(mach_menu[n], is_selected))
                    mach_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }ImGui::EndCombo();

        } ImGui::PopItemWidth();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region
        ImGui::Dummy(ImVec2(0.0f, 5.0f));  // height to top of page
        ImGui::PushItemWidth(scaledElementSize);
        ImGui::SetWindowFontScale(fontSize);
        //ImGui::Dummy(ImVec2(0.0f, 5.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 10.0f));

#pragma endregion --------- formatting ---

            //This is not used anywhere yet other than radio buttons.
            enum AircraftType { None, F18, F15, A10 };    
            static int aircraft = F18; // Default set to F18 here... 

            //////////////////////
            // FUEL variables: ///
            //////////////////////
            static bool b_internal = true;
            static bool b_left = false;
            static bool b_center = false;
            static bool b_right = false;
            const static float internal = 10980.0f;
            const static float left = 2250.0f;
            const static float center = 2250.0f;
            const static float right = 2250.0f;

                // array - added via checkbox logic; internal, left, center, right tanks, in that order.
            static float presetFuel[4] = { 0.0f,0.0f,0.0f,0.0f };
                // Sets the slider max based on drop tanks etc.
            static float maxFuel = 0.0f; 
                // Check to reset the fuel slider due to interaction requirement. Just selecting drop tanks is not enough.
            static bool b_fuelChanged = false;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

            ImGui::BeginChild("Options##", ImVec2(1210, 60), true, ImGuiWindowFlags_NoScrollbar);
            ImGui::PushItemWidth(60);

            // Choose aircraft:
            ImGui::Text("Aircraft Profile: ");
            ImGui::RadioButton("F-18C", &aircraft, F18); ImGui::SameLine();
            ImGui::RadioButton("F-15E", &aircraft, F15); ImGui::SameLine();
            ImGui::RadioButton("A-10CII", &aircraft, A10); ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

            // Check boxes to configure the jet and set max fuel capacity
            ImGui::Text("Fuel Tank Configuration: "); ImGui::SameLine();
            if (ImGui::Checkbox("Internal", &b_internal)) b_fuelChanged = true; ImGui::SameLine();

            if (ImGui::Checkbox("Left Ext.", &b_left)) b_fuelChanged = true; ImGui::SameLine();

            if (ImGui::Checkbox("Center Ext.", &b_center)) b_fuelChanged = true; ImGui::SameLine();

            if (ImGui::Checkbox("Right Ext.", &b_right)) b_fuelChanged = true; ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

                // Logic/Params for checkboxes, sets fuel slider
                {
                    b_internal ?  presetFuel[0] = internal : presetFuel[0] = 0;

                    b_left ? presetFuel[1] = left : presetFuel[1] = 0;

                    b_center ? presetFuel[2] = center : presetFuel[2] = 0;

                    b_right ? presetFuel[3] = right : presetFuel[3] = 0;

                    maxFuel = presetFuel[0] + presetFuel[1] + presetFuel[2] + presetFuel[3];
                }

                // This avoids a bug problem with the fuel slider if the slider doesn't get interacted after checkboxes set.
                if (b_fuelChanged)
                {
                    startFuel = maxFuel;
                    b_fuelChanged = false;
                }

                // Fuel state, modified by checkboxes for configuration
            ImGui::Text("Starting Fuel (LBS)"); ImGui::SameLine(); ImGui::SliderFloat("##", &startFuel, 0, maxFuel, "%.0f"); ImGui::SameLine();


            static float fuelPercent = 0.0f;

            fuelPercent = ((startFuel / maxFuel) * 100);

                // display the fuel percentage with format and bug handling
                // various spaces are added to try to stop the screen 
                // elements jumping around with 1 - 3 digit widths.

                // Handle div 0 case
            if (maxFuel == 0)
            {
                ImGui::Text("(   0 %%)"); ImGui::SameLine(); 
            }

            else
            {       // Formatting due to width of digits: (added spaces)
                if (fuelPercent == 100.0f)
                {
                    ImGui::Text("(%.0f %%)", fuelPercent); ImGui::SameLine();
                }

                else if (fuelPercent >= 0.0f && fuelPercent < 10.0f)
                {
                    ImGui::Text("(    %.0f %%)", fuelPercent); ImGui::SameLine();

                }

                else
                {
                    ImGui::Text("(  %.0f %%)", fuelPercent); ImGui::SameLine();
                }
            } ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

            // Manual fuel flow entry:
            fuelFlow = F18_FuelDataLB_NM [mach_current_idx][alt_current_idx];
            
            ImGui::InputFloat("Fuel Flow, LBS/nm", &fuelFlow, NULL, NULL, "%.0f");
            
            
                // if ()
                //b_valueChanged = true;


            ImGui::PopItemWidth();
            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();

            if (b_debug)
            {
                item.drawDebugInfo();
            }
            
            if (b_debug)
                WhileLoop_testMarker++;

#pragma region

        //ImGui::Dummy(ImVec2(0.0f, 10.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Make the button inside the push tags an obvious color
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.25f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.25f, 1.0f, 1.0f));

#pragma endregion --------- formatting ---
        
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));

        ImGui::BeginChild("Buttons##", ImVec2(1210, 40), true, ImGuiWindowFlags_NoScrollbar);

            // MIDDLE: DRAW WAYPOINT ELEMENTS
            // EACH ' + ' BUTTON CLICK CREATES NEW WAYPOINT ENTRY and adds to DrawEntry loop:
            ImGui::Dummy(ImVec2(68.0f, 00.0f)); ImGui::SameLine();
            if (ImGui::Button(" + ") && (WaypointCounter < 100)) // button click size over-run guard!
            {
                WaypointEntry.emplace_back();
                WaypointCounter++; // Keep track of new waypoint items. Not the same as waypoint ID

                b_valueChanged = true;
            }ImGui::SameLine(); 

            ImGui::Text(" Add Waypoint (%d total, MAX = 100)", WaypointCounter); ImGui::SameLine(); ImGui::Dummy(ImVec2(640.0f, 00.0f)); ImGui::SameLine();
            if (ImGui::Button("LOAD DEMO DATA"))
                b_loadData = true; ImGui::SameLine(); ImGui::Dummy(ImVec2(5.0f, 00.0f)); ImGui::SameLine();
            if (ImGui::Button("CLEAR ALL"))
                WaypointEntry.clear();

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();

#pragma region

            ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 00.0f)); ImGui::SameLine(); ImGui::Dummy(ImVec2(0.0f,30.0f)); ImGui::PopStyleColor(3);// Gives height clearance for the buttons.

#pragma endregion --------- formatting ---

            // MAIN DRAW HERE:
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));

            ImGui::BeginChild("##WaypointChildren", ImVec2(1210, 325), true);
            ImGui::SetWindowFontScale(0.70f);
                item.DrawMain();    // all draw and calc done in MathFunctions.h file....loop contents from here have been moved.

            ImGui::EndChild();

            ImGui::PopStyleVar(); ImGui::PopStyleVar();

            // FOOTER: (TOTALS at bottom of main page)
            if (g_vectorPOS > 1)
            { 
                item.doTotals();    // Do totals calc to supply page footer with info:

#pragma region

            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.7f, 0.3f));ImGui::Dummy(ImVec2(0.0f, 50.0f));ImGui::Dummy(ImVec2(40.0f, 0.0f)); ImGui::SameLine();

#pragma endregion --------- formatting ---

            ImGui::InputInt("Total Fuel Used (LBS)", &totalFuelUsed, NULL, NULL);  ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("Total Distance", &totalDistance, NULL, NULL, "%.0f"); ImGui::SameLine(); ImGui::Text("%s", unitString); ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputInt("Fuel Remaining (LBS)", &fuelRemaining, NULL, NULL);

            ImGui::PopStyleColor();
            }

        ImGui::PopItemWidth();

        ImGui::End();
    }
};

// Starting main window
Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "DCS Mission Fuel Planner";
    spec.Height = 1000;
    spec.Width = 1600;
    

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();

    app->SetMenubarCallback([app]()
        {
            ImGui::BeginMainMenuBar();      // Top Level App window menubar

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    app->Close();
                }
                ImGui::EndMenu();
            } ImGui::Dummy(ImVec2(10.0f, 0.0f));

            if (ImGui::BeginMenu("Settings"))
            {
                (ImGui::MenuItem("Show Debug", "", &b_debug));
                (ImGui::MenuItem("Allow Idling", "", &b_allowIdling));
                if((ImGui::MenuItem("Clear All Data", "", nullptr)))
                    WaypointEntry.clear();
                (ImGui::MenuItem("Load Default", "", &b_loadData));


                if (ImGui::BeginMenu("Set Units"))
                {

                    ImGui::Text("Distance");
                    if ((ImGui::RadioButton("KM", &units, KM)))
                        b_valueChanged = true;
                    if ((ImGui::RadioButton("NM", &units, NM)))
                        b_valueChanged = true;;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Font Size"))
                {
                    ImGui::PushItemWidth(scaledElementSize);

                    if (ImGui::Button("Reset:"))    // simple font reset
                        fontSize = 0.7f;
                    ImGui::SameLine();
                    ImGui::SliderFloat("Font Size", &fontSize, 0.6f, 1.2f, "%.2f");
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            } ImGui::Dummy(ImVec2(10.0f, 0.0f));

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::BeginMenu("About "))
                {
                    ImGui::Text("Built using Dear ImGui framework");
                    ImGui::Text("Using 'Walnut' from Studio Cherno.");
                    ImGui::Text("https://github.com/StudioCherno/Walnut");

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Versions "))
                {
                    ImGui::Text("ImGui Version (%s) (%d) and ", IMGUI_VERSION, IMGUI_VERSION_NUM);
                    ImGui::Text("DCS Fuel Planner Version 1.00, Aug. 2024");
                    ImGui::Text("Author: Keebler750");
                    ImGui::Text("https://github.com/Keebler750/WalnutBase");

                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            } ImGui::Dummy(ImVec2(10.0f, 0.0f));

            ImGui::EndMainMenuBar();
        });

    return app;
}