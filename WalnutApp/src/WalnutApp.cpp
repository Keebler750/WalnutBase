#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"


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

            // some options:
        if (show_demo_window)
            ImGui::ShowDemoWindow();

        if (show_ID_StackTool)
            ImGui::ShowStackToolWindow();

            // Set by bool above, this is a layout item; also needs the WindowFlags re-enabled in "Begin"
            // This section conforms the child to the main window dimensions.
        if (lockWindow) 
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);     // These settings, instead of 'Pos' and 'Size'... 
            ImGui::SetNextWindowSize(viewport->WorkSize);   //avoid drawing over main menubar.
        }

        // Create a window inside main window called "Application Window" and append into it.
        // un-comment the windowFlags when the "lockWindow = true" above.

        //ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.12f, 0.14f, 0.16f, 0.6f) );
        ImGui::Begin("##MainAppWindowFuelCalc", NULL 
                    ,ImGuiWindowFlags_AlwaysHorizontalScrollbar |
                    ImGuiWindowFlags_AlwaysVerticalScrollbar |
                    //ImGuiWindowFlags_NoTitleBar |
                    //ImGuiWindowFlags_NoDocking |
                    ImGuiWindowFlags_MenuBar |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove
                    ); //ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        //    ImGui::Text("Using Dear_ImGui Version: (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        //ImGui::Dummy(ImVec2(0.0f, 20.0f));
        //    ImGui::Text("Set Options:");
            
        ImGui::PushItemWidth(scaledElementSize);
        
            //if (ImGui::Button("Reset:"))    // simple font reset
            //    fontSize = 0.7f;
            //ImGui::SameLine();
            //ImGui::SliderFloat("Font Size", &fontSize, 0.6f, 1.2f, "%.2f");
            ImGui::SetWindowFontScale(fontSize);
            //ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();


            // UNITS are set here:
            //ImGui::Text("Units: "); ImGui::SameLine();
            //if(ImGui::RadioButton("KM", &units, KM))
            //    b_valueChanged = true; ImGui::SameLine();
            //if(ImGui::RadioButton("NM", &units, NM))
            //    b_valueChanged =true;
            //ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
            //ImGui::Checkbox("Show Debug", &b_debug);
            //ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
            //ImGui::Checkbox("Allow Idling", &b_allowIdling);

            if(b_allowIdling)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //This is not used anywhere yet other than radio buttons.
            enum AircraftType { None, F18, F15, A10 };    
            static int aircraft = F18; // Default set to F18 here... 

            // FUEL variables: ///
            //////////////////////
            //static float startFuel = 0; // Moved to header file...
            static bool b_internal = true;
            static bool b_left = false;
            static bool b_center = false;
            static bool b_right = false;
            const static float internal = 10980.0f;
            const static float left = 2250.0f;
            const static float center = 2250.0f;
            const static float right = 2250.0f;
                // added via checkbox logic; internal, left, center, right tanks, in that order.
            static float presetFuel[4] = { 0.0f,0.0f,0.0f,0.0f };
                // Sets the slider max based on drop tanks etc.
            static float maxFuel = 0.0f; 
            static float BugCheckFuel;

                // Choose aircraft:
            ImGui::Text("Aircraft Profile: ");
            ImGui::RadioButton("F-18C", &aircraft, F18); ImGui::SameLine();
            ImGui::RadioButton("F-15E", &aircraft, F15); ImGui::SameLine();
            ImGui::RadioButton("A-10CII", &aircraft, A10); ImGui::SameLine();

                // FUEL SETTINGS: ////////////////////////////////////
                // Set max fuel in slider according to drop tanks, etc.
            maxFuel = presetFuel[0] + presetFuel[1] + presetFuel[2] + presetFuel[3];

                // This avoids a bug problem with the fuel slider if the slider doesn't get interacted after checkboxes.
            if (BugCheckFuel != maxFuel) startFuel = maxFuel;  
            BugCheckFuel = maxFuel;

            ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

                // Check boxes to configure the jet and set max fuel capacity
            ImGui::Text("Fuel Tank Configuration: "); ImGui::SameLine();
            ImGui::Checkbox("Internal", &b_internal);
            ImGui::SameLine();
            ImGui::Checkbox("Left Ext.", &b_left);
            ImGui::SameLine();
            ImGui::Checkbox("Center Ext.", &b_center);
            ImGui::SameLine();
            ImGui::Checkbox("Right Ext.", &b_right);
            ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

                        // Logic/Params for checkboxes above, for clean up purposes:
                        {
                            if (b_internal)
                                presetFuel[0] = internal;
                            else if (!b_internal)
                                presetFuel[0] = 0;

                            if (b_left)
                                presetFuel[1] = left;
                            else if (!b_left)
                                presetFuel[1] = 0;

                            if (b_center)
                                presetFuel[2] = center;
                            else if (!b_center)
                                presetFuel[2] = 0;

                            if (b_right)
                                presetFuel[3] = right;
                            else if (!b_right)
                                presetFuel[3] = 0;
                        }

                // Fuel state, modified by checkboxes for configuration
            ImGui::Text("Starting Fuel State (LBS)"); ImGui::SameLine();
            ImGui::SliderFloat("##", &startFuel, 0, maxFuel, "%.0f"); ImGui::SameLine();

                // display the fuel percentage with format and bug handling
                // various spaces are added to try to stop the screen 
                // elements jumping around with 1 - 3 digit widths.
            static float fuelPercent = 0.0f;

            fuelPercent = ((startFuel / maxFuel) * 100);

            // Handle div 0 case
            if (maxFuel == 0)
            {
                ImGui::Text("(     0 %%)"); ImGui::SameLine(); 
            }
            else
            {
                if (fuelPercent == 100.0f)
                {
                    ImGui::Text("(%.0f %%)", fuelPercent); ImGui::SameLine();
                }
                else
                {
                    ImGui::Text("(  %.0f %%)", fuelPercent); ImGui::SameLine();
                }
            }

            ImGui::Dummy(ImVec2(45.0f, 0.0f)); ImGui::SameLine();
            if (ImGui::InputFloat("Fuel Flow, LBS/nm", &fuelFlow, NULL, NULL, "%.0f"))
                b_valueChanged = true;

            if (b_debug)
            {
                ImGui::Dummy(ImVec2(0.0f, 10.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 0.0f));

                ImGui::Text("DEBUG TEST for Looping Function Increments:"); ImGui::SameLine(); ImGui::Text("  While - %d  ", WhileLoop_testMarker); ImGui::SameLine(); ImGui::Text("  HAV - %d  ", Haversine_testMarker);
                ImGui::SameLine(); ImGui::Text("  Clamp1 - %d  ", ClampFloat1_testMarker); ImGui::SameLine(); ImGui::Text("  Clamp2 - %d  ", ClampFloat2_testMarker); ImGui::SameLine(); ImGui::Text("  Draw - %d  ", DrawEntry_testMarker);
            }
            
            WhileLoop_testMarker++;

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); ImGui::Separator(); ImGui::Dummy(ImVec2(0.0f, 10.0f));
        // MIDDLE: DRAW WAYPOINT ELEMENTS

            // Make the button inside the push tags an obvious color
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.25f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.25f, 1.0f, 1.0f));


            // EACH ' + ' BUTTON CLICK CREATES NEW WAYPOINT ENTRY and adds to DrawEntry loop:
            if (ImGui::Button(" + ") && (WaypointCounter < 100)) // button click size over-run guard!
            {
                WaypointCounter++;  // Keep track of new waypoint items. Not the same as waypoint ID
                b_IsNewWaypoint = true; // Only Emplace_back on button click for new waypoint
            }ImGui::SameLine();

            ImGui::Text(" Add Waypoint (%d total, MAX = 100)", WaypointCounter);

            ImGui::Dummy(ImVec2(0.0f,10.0f));

        ImGui::PopStyleColor(3);

            // This is used as the position index inside the vector of waypoint data.
            static int VectorPosition = 0;

            for (VectorPosition = 0; VectorPosition < WaypointCounter; VectorPosition++)
            {
                // Note - ITEM is instantiated in EntryPoint.h before the while-loop.
                item.drawEntry(VectorPosition, units); // Modular draw call, per the number of waypoints
            }

        // FOOTER: (TOTALS at bottom of main page)

            if (VectorPosition >= 2)
            {
                item.totalFuelCalc();
                item.totalDistanceCalc();
                item.fuelRemainingCalc();

            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.25f, 0.7f, 0.7f, 0.3f));

                ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 30.0f)); //ImGui::SameLine();
                ImGui::InputInt("Total Fuel Used (LBS)", &totalFuelUsed, NULL, NULL);  ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("Total Distance (UNITS)", &totalDistance, NULL, NULL, "%.0f"); ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();
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
