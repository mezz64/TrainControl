#include "ui.h"

//UI handles
uint16_t traindirectionlabel[2], trainSlider[2], trainPWM[2];
uint16_t soundlabel, volSlider;

uint16_t wifi_ssid_text, wifi_pass_text;

uint16_t mainLabel, mainSwitcher, mainSlider, mainText, mainNumber, mainScrambleButton, mainTime;
uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2, styleSlider2;
uint16_t graph;
volatile bool updates = false;

// This is the main function which builds our GUI
void setUpUI() {
	//Turn off verbose debugging
	ESPUI.setVerbosity(Verbosity::Quiet);

	//Make sliders continually report their position as they are being dragged.
	ESPUI.sliderContinuous = true;

	//This GUI is going to be a tabbed GUI, so we are adding most controls using ESPUI.addControl
	//which allows us to set a parent control. If we didn't need tabs we could use the simpler add
	//functions like:
	//    ESPUI.button()
	//    ESPUI.label()


	/*
	 * Tab: Basic Controls
	 * This tab contains all the basic ESPUI controls, and shows how to read and update them at runtime.
	 *-----------------------------------------------------------------------------------------------------------*/
	auto maintab = ESPUI.addControl(Tab, "", "Train Controls");

    //"background-color: unset; width: 50%;";
    String tgroupLabelStyle = "width: 30%;";
    String clearLabelStyle = "background-color: unset; width: 100%;";

	// Train A Control
	auto groupAlabel = ESPUI.addControl(Label, "Train A", "", Emerald, maintab);
	ESPUI.setElementStyle(groupAlabel, clearLabelStyle);
    traindirectionlabel[A] = ESPUI.addControl(Label, "", "Direction", Emerald, groupAlabel);
	ESPUI.setElementStyle(traindirectionlabel[A], tgroupLabelStyle);
	ESPUI.addControl(Button, "a_direction", "Change Direction", Alizarin, groupAlabel, generalCallback);
    trainSlider[A] = ESPUI.addControl(Slider, "a_slider", "0", Turquoise, groupAlabel, generalCallback);
	ESPUI.addControl(Min, "", "0", None, trainSlider[A]);
	ESPUI.addControl(Max, "", "100", None, trainSlider[A]);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "Train A Speed", None, groupAlabel), clearLabelStyle);
    // ESPUI.setPanelStyle(groupAlabel, "min-width: 411px;");

	// Train B Control
    auto groupBlabel = ESPUI.addControl(Label, "Train B", "", Turquoise, maintab);
	ESPUI.setElementStyle(groupBlabel, clearLabelStyle);
    traindirectionlabel[B] = ESPUI.addControl(Label, "", "Direction", Emerald, groupBlabel);
	ESPUI.setElementStyle(traindirectionlabel[B], tgroupLabelStyle);
	ESPUI.addControl(Button, "b_direction", "Change Direction", Alizarin, groupBlabel, generalCallback);
    trainSlider[B] = ESPUI.addControl(Slider, "b_slider", "0", Turquoise, groupBlabel, generalCallback);
	ESPUI.addControl(Min, "", "0", None, trainSlider[B]);
	ESPUI.addControl(Max, "", "100", None, trainSlider[B]);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "Train B Speed", None, groupBlabel), clearLabelStyle);
    // ESPUI.setPanelStyle(groupBlabel, "min-width: 411px;");

	// Sound Control
    auto groupSlabel = ESPUI.addControl(Label, "Sound Control", "", Carrot, maintab);
	ESPUI.setElementStyle(groupSlabel, clearLabelStyle);
	soundlabel = ESPUI.addControl(Label, "", "Now Playing", Emerald, groupSlabel);
	ESPUI.addControl(Button, "horn01_button", "Horn 1", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "bell01_button", "Bell 1", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "crazy_button", "Crazy", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "whistle01_button", "Whistle 1", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "whistle02_button", "Whistle 2", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "whistle03_button", "Whistle 3", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "whistle04_button", "Whistle 4", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "misc01_button", "Crossing", Alizarin, groupSlabel, generalCallback);
	ESPUI.addControl(Button, "misc02_button", "Chugg", Alizarin, groupSlabel, generalCallback);


	// ESPUI.setElementStyle(grouplabel2, "font-size: x-large; font-family: serif;");

	// ESPUI.addControl(Separator, "General controls", "", None, maintab);
	// ESPUI.addControl(Button, "Button", "Button 1", Alizarin, maintab, extendedCallback, (void*)19);
	// mainLabel = ESPUI.addControl(Label, "Label", "Label text", Emerald, maintab, generalCallback);
	// mainSwitcher = ESPUI.addControl(Switcher, "Switcher", "", Sunflower, maintab, generalCallback);

	// //Sliders default to being 0 to 100, but if you want different limits you can add a Min and Max control
	// mainSlider = ESPUI.addControl(Slider, "Slider", "200", Turquoise, maintab, generalCallback);
	// ESPUI.addControl(Min, "", "10", None, mainSlider);
	// ESPUI.addControl(Max, "", "400", None, mainSlider);

	//These are the values for the selector's options. (Note that they *must* be declared static
	//so that the storage is allocated in global memory and not just on the stack of this function.)
	// static String optionValues[] {"Value 1", "Value 2", "Value 3", "Value 4", "Value 5"};
	// auto mainselector = ESPUI.addControl(Select, "Selector", "Selector", Wetasphalt, maintab, generalCallback);
	// for(auto const& v : optionValues) {
	// 	ESPUI.addControl(Option, v.c_str(), v, None, mainselector);
	// }

	// mainText = ESPUI.addControl(Text, "Text Input", "Initial value", Alizarin, maintab, generalCallback);

	// //Number inputs also accept Min and Max components, but you should still validate the values.
	// mainNumber = ESPUI.addControl(Number, "Number Input", "42", Emerald, maintab, generalCallback);
	// ESPUI.addControl(Min, "", "10", None, mainNumber);
	// ESPUI.addControl(Max, "", "50", None, mainNumber);

	// ESPUI.addControl(Separator, "Updates", "", None, maintab);

	// //This button will update all the updatable controls on this tab to random values
	// mainScrambleButton = ESPUI.addControl(Button, "Scramble Values", "Scramble Values", Carrot, maintab, scrambleCallback);
	// ESPUI.addControl(Switcher, "Constant updates", "0", Carrot, maintab, updateCallback);
	// mainTime = ESPUI.addControl(Time, "", "", None, 0, generalCallback);
	// ESPUI.addControl(Button, "Get Time", "Get Time", Carrot, maintab, getTimeCallback);

	// ESPUI.addControl(Separator, "Control Pads", "", None, maintab);
	// ESPUI.addControl(Pad, "Normal", "", Peterriver, maintab, generalCallback);
	// ESPUI.addControl(PadWithCenter, "With center", "", Peterriver, maintab, generalCallback);

	/*
	 * Tab: Settings
	 * This tab allows some setting changes
	 *-----------------------------------------------------------------------------------------------------------*/
	String pwmgroupLabelStyle = "width: 50%;";
	String wideNumStyle = "margin: 0px 45px;";
	String numLabelStyle = "background-color: unset; width: 15%; text-align: left;";
	auto settingstab = ESPUI.addControl(Tab, "", "Settings");

	auto groupVlabel = ESPUI.addControl(Label, "Volume Control", "", Carrot, settingstab);
	ESPUI.setElementStyle(groupVlabel, clearLabelStyle);
	volSlider = ESPUI.addControl(Slider, "v_slider", "0", Turquoise, groupVlabel, generalCallback);
	ESPUI.addControl(Min, "", "0", None, volSlider);
	ESPUI.addControl(Max, "", "30", None, volSlider);
	// ESPUI.setVertical(volSlider);

	auto groupPlabel = ESPUI.addControl(Label, "PWM Control", "", Alizarin, settingstab);
	ESPUI.setElementStyle(groupPlabel, clearLabelStyle);
	// trainPWM[A] = ESPUI.addControl(Text, "Train A PWM Frequency (Hz)", "", Alizarin, groupPlabel, textCallback);
	trainPWM[A] = ESPUI.addControl(Number, "Train A PWM Frequency (Hz)", "", Alizarin, groupPlabel, generalCallback);
	ESPUI.addControl(Min, "", "100", None, trainPWM[A]);
	ESPUI.addControl(Max, "", "2000", None, trainPWM[A]);
	// ESPUI.setElementStyle(trainPWM[A], wideNumStyle);
	ESPUI.setElementStyle(ESPUI.addControl(Label, "Hz", "Hz", Carrot, groupPlabel), numLabelStyle);
	ESPUI.setElementStyle(ESPUI.addControl(Button, "afreq_button", "Set Train A Frequency", Alizarin, groupPlabel, generalCallback), pwmgroupLabelStyle);

	// trainPWM[B] = ESPUI.addControl(Text, "Train B PWM Frequency (Hz)", "", Alizarin, groupPlabel, textCallback);
	trainPWM[B] = ESPUI.addControl(Number, "Train B PWM Frequency (Hz)", "", Alizarin, groupPlabel, generalCallback);
	ESPUI.addControl(Min, "", "100", None, trainPWM[B]);
	ESPUI.addControl(Max, "", "2000", None, trainPWM[B]);
	// ESPUI.setElementStyle(trainPWM[B], wideNumStyle);
	ESPUI.setElementStyle(ESPUI.addControl(Label, "Hz", "Hz", Carrot, groupPlabel), numLabelStyle);
	ESPUI.setElementStyle(ESPUI.addControl(Button, "bfreq_button", "Set Train B Frequency", Alizarin, groupPlabel, generalCallback), pwmgroupLabelStyle);

	/*
	 * Tab: Colours
	 * This tab shows all the basic colours
	 *-----------------------------------------------------------------------------------------------------------*/
	auto colourtab = ESPUI.addControl(Tab, "", "Colours");
	ESPUI.addControl(Button, "Alizarin", "Alizarin", Alizarin, colourtab, generalCallback);
	ESPUI.addControl(Button, "Turquoise", "Turquoise", Turquoise, colourtab, generalCallback);
	ESPUI.addControl(Button, "Emerald", "Emerald", Emerald, colourtab, generalCallback);
	ESPUI.addControl(Button, "Peterriver", "Peterriver", Peterriver, colourtab, generalCallback);
	ESPUI.addControl(Button, "Wetasphalt", "Wetasphalt", Wetasphalt, colourtab, generalCallback);
	ESPUI.addControl(Button, "Sunflower", "Sunflower", Sunflower, colourtab, generalCallback);
	ESPUI.addControl(Button, "Carrot", "Carrot", Carrot, colourtab, generalCallback);
	ESPUI.addControl(Button, "Dark", "Dark", Dark, colourtab, generalCallback);


	/*
	 * Tab: Styled controls
	 * This tab shows off how inline CSS styles can be applied to elements and panels in order
	 * to customise the look of the UI.
	 *-----------------------------------------------------------------------------------------------------------*/
	// auto styletab = ESPUI.addControl(Tab, "", "Styled controls");
	// styleButton = ESPUI.addControl(Button, "Styled Button", "Button", Alizarin, styletab, generalCallback);
	// styleLabel = ESPUI.addControl(Label, "Styled Label", "This is a label", Alizarin, styletab, generalCallback);
	// styleSwitcher = ESPUI.addControl(Switcher, "Styled Switcher", "1", Alizarin, styletab, generalCallback);
	// styleSlider = ESPUI.addControl(Slider, "Styled Slider", "0", Alizarin, styletab, generalCallback);

	// //This button will randomise the colours of the above controls to show updating of inline styles
	// ESPUI.addControl(Button, "Randomise Colours", "Randomise Colours", Sunflower, styletab, styleCallback);

	// ESPUI.addControl(Separator, "Other styling examples", "", None, styletab);
	// styleButton2 = ESPUI.addControl(Button, "Styled Button", "Button", Alizarin, styletab, generalCallback);
	// ESPUI.setPanelStyle(styleButton2, "background: linear-gradient(90deg, rgba(131,58,180,1) 0%, rgba(253,29,29,1) 50%, rgba(252,176,69,1) 100%); border-bottom: #555;");
	// ESPUI.setElementStyle(styleButton2, "border-radius: 2em; border: 3px solid black; width: 30%; background-color: #8df;");

	// styleSlider2 = ESPUI.addControl(Slider, "Styled Slider", "0", Dark, styletab, generalCallback);
	// ESPUI.setElementStyle(styleSlider2, "background: linear-gradient(to right, red, orange, yellow, green, blue);");

	// styleLabel2 = ESPUI.addControl(Label, "Styled Label", "This is a label", Dark, styletab, generalCallback);
	// ESPUI.setElementStyle(styleLabel2, "text-shadow: 3px 3px #74b1ff, 6px 6px #c64ad7; font-size: 60px; font-variant-caps: small-caps; background-color: unset; color: #c4f0bb; -webkit-text-stroke: 1px black;");


	/*
	 * Tab: Grouped controls
	 * This tab shows how multiple control can be grouped into the same panel through the use of the
	 * parentControl value. This also shows how to add labels to grouped controls, and how to use vertical controls.
	//  *-----------------------------------------------------------------------------------------------------------*/
	// auto grouptab = ESPUI.addControl(Tab, "", "Grouped controls");

	// //The parent of this button is a tab, so it will create a new panel with one control.
	// auto groupbutton = ESPUI.addControl(Button, "Button Group", "Button A", Dark, grouptab, generalCallback);
	// //However the parent of this button is another control, so therefore no new panel is
	// //created and the button is added to the existing panel.
	// ESPUI.addControl(Button, "", "Button B", Alizarin, groupbutton, generalCallback);
	// ESPUI.addControl(Button, "", "Button C", Alizarin, groupbutton, generalCallback);


	// //Sliders can be grouped as well
	// //To label each slider in the group, we are going add additional labels and give them custom CSS styles
	// //We need this CSS style rule, which will remove the label's background and ensure that it takes up the entire width of the panel
	// // String clearLabelStyle = "background-color: unset; width: 100%;";
	// //First we add the main slider to create a panel
	// auto groupsliders = ESPUI.addControl(Slider, "Slider Group", "10", Dark, grouptab, generalCallback);
	// //Then we add a label and set its style to the clearLabelStyle. Here we've just given it the name "A"
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, groupsliders), clearLabelStyle);
	// //We can now continue to add additional sliders and labels
	// ESPUI.addControl(Slider, "", "20", None, groupsliders, generalCallback);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, groupsliders), clearLabelStyle);
	// ESPUI.addControl(Slider, "", "30", None, groupsliders, generalCallback);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, groupsliders), clearLabelStyle);

	// //We can also usefully group switchers.
	// auto groupswitcher = ESPUI.addControl(Switcher, "Switcher Group", "0", Dark, grouptab, generalCallback);
	// ESPUI.addControl(Switcher, "", "1", Sunflower, groupswitcher, generalCallback);
	// ESPUI.addControl(Switcher, "", "0", Sunflower, groupswitcher, generalCallback);
	// ESPUI.addControl(Switcher, "", "1", Sunflower, groupswitcher, generalCallback);
	// //To label these switchers we need to first go onto a "new line" below the line of switchers
	// //To do this we add an empty label set to be clear and full width (with our clearLabelStyle)
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, groupswitcher), clearLabelStyle);
	// //We will now need another label style. This one sets its width to the same as a switcher (and turns off the background)
	// String switcherLabelStyle = "width: 60px; margin-left: .3rem; margin-right: .3rem; background-color: unset;";
	// //We can now just add the styled labels.
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, groupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, groupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, groupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, groupswitcher), switcherLabelStyle);

	// //You can mix and match different control types, but the results might sometimes
	// //need additional styling to lay out nicely.
	// auto grouplabel = ESPUI.addControl(Label, "Mixed Group", "Main label", Dark, grouptab);
	// auto grouplabel2 = ESPUI.addControl(Label, "", "Secondary label", Emerald, grouplabel);
	// ESPUI.addControl(Button, "", "Button D", Alizarin, grouplabel, generalCallback);
	// ESPUI.addControl(Switcher, "", "1", Sunflower, grouplabel, generalCallback);
	// ESPUI.setElementStyle(grouplabel2, "font-size: x-large; font-family: serif;");

	//Some controls can even support vertical orientation, currently Switchers and Sliders
	// ESPUI.addControl(Separator, "Vertical controls", "", None, grouptab);
	// auto vertgroupswitcher = ESPUI.addControl(Switcher, "Vertical Switcher Group", "0", Dark, grouptab, generalCallback);
	// ESPUI.setVertical(vertgroupswitcher); 
	// //On the following lines we wrap the value returned from addControl and send it straight to setVertical
	// ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
	// ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
	// ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
	// //The mechanism for labelling vertical switchers is the same as we used above for horizontal ones
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, vertgroupswitcher), clearLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, vertgroupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, vertgroupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, vertgroupswitcher), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, vertgroupswitcher), switcherLabelStyle);

	// auto vertgroupslider = ESPUI.addControl(Slider, "Vertical Slider Group", "15", Dark, grouptab, generalCallback);
	// ESPUI.setVertical(vertgroupslider);
	// ESPUI.setVertical(ESPUI.addControl(Slider, "", "25", None, vertgroupslider, generalCallback));
	// ESPUI.setVertical(ESPUI.addControl(Slider, "", "35", None, vertgroupslider, generalCallback));
	// ESPUI.setVertical(ESPUI.addControl(Slider, "", "45", None, vertgroupslider, generalCallback));
	// //The mechanism for labelling vertical sliders is the same as we used above for switchers
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, vertgroupslider), clearLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, vertgroupslider), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, vertgroupslider), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, vertgroupslider), switcherLabelStyle);
	// ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, vertgroupslider), switcherLabelStyle);

	//Note that combining vertical and horizontal sliders is going to result in very messy layout!

	/*
	 * Tab: Example UI
	 * An example UI for the documentation
	 *-----------------------------------------------------------------------------------------------------------*/
	// auto exampletab = ESPUI.addControl(Tab, "Example", "Example");
	// ESPUI.addControl(Separator, "Control and Status", "", None, exampletab);
	// ESPUI.addControl(Switcher, "Power", "1", Alizarin, exampletab, generalCallback);
	// ESPUI.addControl(Label, "Status", "System status: OK", Wetasphalt, exampletab, generalCallback);
	
	// ESPUI.addControl(Separator, "Settings", "", None, exampletab);
	// ESPUI.addControl(PadWithCenter, "Attitude Control", "", Dark, exampletab, generalCallback);
	// auto examplegroup1 = ESPUI.addControl(Button, "Activate Features", "Feature A", Carrot, exampletab, generalCallback);
	// ESPUI.addControl(Button, "Activate Features", "Feature B", Carrot, examplegroup1, generalCallback);
	// ESPUI.addControl(Button, "Activate Features", "Feature C", Carrot, examplegroup1, generalCallback);
	// ESPUI.addControl(Slider, "Value control", "45", Peterriver, exampletab, generalCallback);

	/*
	 * Tab: WiFi Credentials
	 * You use this tab to enter the SSID and password of a wifi network to autoconnect to.
	 *-----------------------------------------------------------------------------------------------------------*/
	// auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
	// wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, textCallback);
	// //Note that adding a "Max" control to a text control sets the max length
	// ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
	// wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, textCallback);
	// ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	// // ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, enterWifiDetailsCallback);


	//Finally, start up the UI. 
	//This should only be called once we are connected to WiFi.
	// ESPUI.begin("TrainControl");
    ESPUI.beginLITTLEFS("TrainControl");
}

//This callback generates and applies inline styles to a bunch of controls to change their colour.
//The styles created are of the form: 
//	"border-bottom: #999 3px solid; background-color: #aabbcc;"
//	"background-color: #aabbcc;"
void styleCallback(Control *sender, int type) {
	//Declare space for style strings. These have to be static so that they are always available
	//to the websocket layer. If we'd not made them static they'd be allocated on the heap and
	//will be unavailable when we leave this function.
	static char stylecol1[60], stylecol2[30]; 
	if(type == B_UP) {
		//Generate two random HTML hex colour codes, and print them into CSS style rules
		sprintf(stylecol1, "border-bottom: #999 3px solid; background-color: #%06X;", (unsigned int) random(0x0, 0xFFFFFF));
		sprintf(stylecol2, "background-color: #%06X;", (unsigned int) random(0x0, 0xFFFFFF));

		//Apply those styles to various elements to show how controls react to styling
		ESPUI.setPanelStyle(styleButton, stylecol1);
		ESPUI.setElementStyle(styleButton, stylecol2);
		ESPUI.setPanelStyle(styleLabel, stylecol1);
		ESPUI.setElementStyle(styleLabel, stylecol2);
		ESPUI.setPanelStyle(styleSwitcher, stylecol1);
		ESPUI.setElementStyle(styleSwitcher, stylecol2);
		ESPUI.setPanelStyle(styleSlider, stylecol1);
		ESPUI.setElementStyle(styleSlider, stylecol2);
	}
}


//This callback updates the "values" of a bunch of controls
void scrambleCallback(Control *sender, int type) {
	static char rndString1[10];
	static char rndString2[20];
	static bool scText = false;

	if(type == B_UP) { //Button callbacks generate events for both UP and DOWN.
		//Generate some random text
		randomString(rndString1, 10);
		randomString(rndString2, 20);

		//Set the various controls to random value to show how controls can be updated at runtime
		ESPUI.updateLabel(mainLabel, String(rndString1));
		ESPUI.updateSwitcher(mainSwitcher, ESPUI.getControl(mainSwitcher)->value.toInt() ? false : true);
		ESPUI.updateSlider(mainSlider, random(10, 400));
		ESPUI.updateText(mainText, String(rndString2));
		ESPUI.updateNumber(mainNumber, random(100000));
		ESPUI.updateButton(mainScrambleButton, scText ? "Scrambled!" : "Scrambled.");
		scText = !scText;
	}
}

void updateCallback(Control *sender, int type) {
	updates = (sender->value.toInt() > 0);
}

void getTimeCallback(Control *sender, int type) {
	if(type == B_UP) {
		ESPUI.updateTime(mainTime);
	}
}

void graphAddCallback(Control *sender, int type) {
	if(type == B_UP) {
		ESPUI.addGraphPoint(graph, random(1, 50));
	}
}

void graphClearCallback(Control *sender, int type) {
	if(type == B_UP) {
		ESPUI.clearGraph(graph);
	}
}


//Most elements in this test UI are assigned this generic callback which prints some
//basic information. Event types are defined in ESPUI.h
void generalCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);

    if (sender->label == "a_direction" && type == -1) {
        directionCHANGED[A] = true;
    }
    if (sender->label == "b_direction" && type == -1) {
        directionCHANGED[B] = true;
    }
    if (sender->label == "a_slider" && type == 8) { // Train A Slider
		uiSPEEDCHANGED[A] = true;
        inputSpeed[A] = map(sender->value.toInt(), 0, 100, 0, speedScale[A]);
    }
    if (sender->label == "b_slider" && type == 8) { // Train B Slider
		uiSPEEDCHANGED[B] = true;
        inputSpeed[B] = map(sender->value.toInt(), 0, 100, 0, speedScale[B]);
    }
	if (sender->label == "v_slider" && type == 8) { // Volume Slider
        // soundVol = map(sender->value.toInt(), 0, 100, 0, 30);
		soundVol = sender->value.toInt();
    }
    if (sender->label == "horn01_button" && type == -1) { // Horn Sound
        playTrack = HORN01;
    }
    if (sender->label == "bell01_button" && type == -1) { // Bell Sound
        playTrack = BELL01;
    }
    if (sender->label == "crazy_button" && type == -1) { // Crazy Sound
        playTrack = CRAZY;
    }
	if (sender->label == "whistle01_button" && type == -1) { // WH01 Sound
        playTrack = WHISTLE01;
    }
	if (sender->label == "whistle02_button" && type == -1) { // WH02 Sound
        playTrack = WHISTLE02;
    }
	if (sender->label == "whistle03_button" && type == -1) { // WH01 Sound
        playTrack = WHISTLE03;
    }
	if (sender->label == "whistle04_button" && type == -1) { //  Sound
        playTrack = WHISTLE04;
    }
	if (sender->label == "misc01_button" && type == -1) { // Crossing Sound
        playTrack = MISC01;
    }
	if (sender->label == "misc02_button" && type == -1) { // Chugg Sound
        playTrack = MISC02;
    }

	if (sender->label == "afreq_button" && type == -1) { // PWM A Setpoint
        trainPWMHz[A] = ESPUI.getControl(trainPWM[A])->value.toInt();
		pwmCHANGED[A] = true;
    }
	if (sender->label == "bfreq_button" && type == -1) { // PWM B Setpoint
        trainPWMHz[B] = ESPUI.getControl(trainPWM[B])->value.toInt();
		pwmCHANGED[B] = true;
    }	

}

// Most elements in this test UI are assigned this generic callback which prints some
// basic information. Event types are defined in ESPUI.h
// The extended param can be used to hold a pointer to additional information
// or for C++ it can be used to return a this pointer for quick access
// using a lambda function
void extendedCallback(Control* sender, int type, void* param)
{
    Serial.print("CB: id(");
    Serial.print(sender->id);
    Serial.print(") Type(");
    Serial.print(type);
    Serial.print(") '");
    Serial.print(sender->label);
    Serial.print("' = ");
    Serial.println(sender->value);
    Serial.println(String("param = ") + String((int)param));
}

void textCallback(Control *sender, int type) {
	//This callback is needed to handle the changed values, even though it doesn't do anything itself.
}

void randomString(char *buf, int len) {
	for(auto i = 0; i < len-1; i++) 
		buf[i] = random(0, 26) + 'A';
	buf[len-1] = '\0';
}
