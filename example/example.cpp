#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wii.h>

using namespace std;

//int LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};

void HandleEvent(Wii &wm) {
    char prefixString[64];

    if(wm.isJustPressed(WIIMOTE_BUTTON_TWO)) {
        wm.Status();
        cout << "battery:" << wm.GetBatteryLevel() << endl;
    }

    if(wm.isHeld(WIIMOTE_BUTTON_ONE)) {
        cout << "battery:" << wm.GetBatteryLevel() << endl;
    }

    if(wm.isJustPressed(WIIMOTE_BUTTON_LEFT)) {
        wm.Set_Leds(WIIMOTE_LED_1);
    }

    if(wm.isJustPressed(WIIMOTE_BUTTON_RIGHT)) {
        wm.Set_Leds(WIIMOTE_LED_4);
    }

    if(wm.isJustPressed(WIIMOTE_BUTTON_UP)) {
        wm.Rumble(1);
    }

    if(wm.isJustPressed(WIIMOTE_BUTTON_DOWN)) {
        wm.ToggleRumble();
    }
}

/*
void HandleEvent(Wii &wm) {
    char prefixString[64];

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_MINUS))
    {
        wm.SetMotionSensingMode(CWiimote::OFF);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_PLUS))
    {
        wm.SetMotionSensingMode(CWiimote::ON);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_DOWN))
    {
        wm.IR.SetMode(CIR::OFF);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_UP))
    {
        wm.IR.SetMode(CIR::ON);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_RIGHT))
    {
        wm.EnableMotionPlus(CWiimote::ON);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_LEFT))
    {
     	 wm.EnableMotionPlus(CWiimote::OFF);
    }

    if(wm.Buttons.isJustPressed(CButtons::BUTTON_B))
    {
        wm.ToggleRumble();
    }

    sprintf(prefixString, "Controller [%i]: ", wm.GetID());

    if(wm.Buttons.isPressed(CButtons::BUTTON_A))
    {
        printf("%s A pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_B))
    {
        printf("%s B pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_UP))
    {
        printf("%s Up pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_DOWN))
    {
        printf("%s Down pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_LEFT))
    {
        printf("%s Left pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_RIGHT))
    {
        printf("%s Right pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_MINUS))
    {
        printf("%s Minus pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_PLUS))
    {
        printf("%s Plus pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_ONE))
    {
        printf("%s One pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_TWO))
    {
        printf("%s Two pressed\n", prefixString);
    }

    if(wm.Buttons.isPressed(CButtons::BUTTON_HOME))
    {
        printf("%s Home pressed\n", prefixString);
    }

    // if the accelerometer is turned on then print angles
    if(wm.isUsingACC())
    {
        float pitch, roll, yaw;
        wm.Accelerometer.GetOrientation(pitch, roll, yaw);
        printf("%s wiimote roll = %f\n", prefixString, roll);
        printf("%s wiimote pitch = %f\n", prefixString, pitch);
        printf("%s wiimote yaw = %f\n", prefixString, yaw);
    }

	// if the Motion Plus is turned on then print angles
    if(wm.isUsingMotionPlus()) {
    	float roll_rate, pitch_rate, yaw_rate;
    	wm.ExpansionDevice.MotionPlus.Gyroscope.GetRates(roll_rate,pitch_rate,yaw_rate);

        printf("%s motion plus roll rate = %f\n", prefixString,roll_rate);
    	printf("%s motion plus pitch rate = %f\n", prefixString,pitch_rate);
    	printf("%s motion plus yaw rate = %f\n", prefixString,yaw_rate);
    }

    // if(IR tracking is on then print the coordinates
    if(wm.isUsingIR())
    {
        std::vector<CIRDot>::iterator i;
        int x, y;
        int index;

        printf("%s Num IR Dots: %i\n", prefixString, wm.IR.GetNumDots());
        printf("%s IR State: %u\n", prefixString, wm.IR.GetState());

        std::vector<CIRDot>& dots = wm.IR.GetDots();

        for(index = 0, i = dots.begin(); i != dots.end(); ++index, ++i)
        {
            if((*i).isVisible())
            {
                (*i).GetCoordinate(x, y);
                printf("%s IR source %i: (%i, %i)\n", prefixString, index, x, y);

                wm.IR.GetCursorPosition(x, y);
                printf("%s IR cursor: (%i, %i)\n", prefixString, x, y);
                printf("%s IR z distance: %f\n", prefixString, wm.IR.GetDistance());
            }
        }
    }

    int exType = wm.ExpansionDevice.GetType();
    if(exType == wm.ExpansionDevice.TYPE_NUNCHUK)
    {
        float pitch, roll, yaw, a_pitch, a_roll;
        float angle, magnitude;

        CNunchuk &nc = wm.ExpansionDevice.Nunchuk;

        sprintf(prefixString, "Nunchuk [%i]: ", wm.GetID());

        if(nc.Buttons.isPressed(CNunchukButtons::BUTTON_C))
        {
            printf("%s C pressed\n", prefixString);
        }

        if(nc.Buttons.isPressed(CNunchukButtons::BUTTON_Z))
        {
            printf("%s Z pressed\n", prefixString);
        }

        nc.Accelerometer.GetOrientation(pitch, roll, yaw);
        printf("%s roll = %f\n", prefixString, roll);
        printf("%s pitch = %f\n", prefixString, pitch);
        printf("%s yaw = %f\n", prefixString, yaw);

        nc.Joystick.GetPosition(angle, magnitude);
        printf("%s joystick angle = %f\n", prefixString, angle);
        printf("%s joystick magnitude = %f\n", prefixString, magnitude);
    }
    else if(exType == wm.ExpansionDevice.TYPE_CLASSIC)
    {
        float angle, magnitude;

        CClassic &cc = wm.ExpansionDevice.Classic;

        sprintf(prefixString, "Classic [%i]: ", wm.GetID());

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_A))
        {
            printf("%s A pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_B))
        {
            printf("%s B pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_X))
        {
            printf("%s X pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_Y))
        {
            printf("%s Y pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_LEFT))
        {
            printf("%s Left pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_UP))
        {
            printf("%s Up pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_RIGHT))
        {
            printf("%s Right pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_DOWN))
        {
            printf("%s Down pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_PLUS))
        {
            printf("%s Plus pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_MINUS))
        {
            printf("%s Minus pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_HOME))
        {
            printf("%s Home pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_ZL))
        {
            printf("%s ZL pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_L))
        {
            printf("%s ZR pressed\n", prefixString);
        }

        if(cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_R))
        {
            printf("%s ZR pressed\n", prefixString);
        }

        printf("%s L button pressed = %f\n", prefixString, cc.GetLShoulderButton());
        printf("%s R button pressed = %f\n", prefixString, cc.GetRShoulderButton());

        cc.LeftJoystick.GetPosition(angle, magnitude);
        printf("%s left joystick angle = %f\n", prefixString, angle);
        printf("%s left joystick magnitude = %f\n", prefixString, magnitude);

        cc.RightJoystick.GetPosition(angle, magnitude);
        printf("%s right joystick angle = %f\n", prefixString, angle);
        printf("%s right joystick magnitude = %f\n", prefixString, magnitude);
    }
    else if(exType == wm.ExpansionDevice.TYPE_GUITAR_HERO_3)
    {
        float angle, magnitude;

        CGuitarHero3 &gh = wm.ExpansionDevice.GuitarHero3;

        sprintf(prefixString, "Guitar [%i]: ", wm.GetID());

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_STRUM_UP))
        {
            printf("%s Strum Up pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_STRUM_DOWN))
        {
            printf("%s Strum Down pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_YELLOW))
        {
            printf("%s Yellow pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_GREEN))
        {
            printf("%s Green pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_BLUE))
        {
            printf("%s Blue pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_RED))
        {
            printf("%s Red pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_ORANGE))
        {
            printf("%s Orange pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_PLUS))
        {
            printf("%s Plus pressed\n", prefixString);
        }

        if(gh.Buttons.isPressed(CGH3Buttons::BUTTON_MINUS))
        {
            printf("%s Minus pressed\n", prefixString);
        }

        printf("%s whammy bar = %f\n", prefixString, gh.GetWhammyBar());

        gh.Joystick.GetPosition(angle, magnitude);
        printf("%s joystick angle = %f\n", prefixString, angle);
        printf("%s joystick magnitude = %f\n", prefixString, magnitude);
    }
	 else if(exType == wm.ExpansionDevice.TYPE_BALANCE_BOARD) 
	 {
		CBalanceBoard &bb = wm.ExpansionDevice.BalanceBoard;
		float total, topLeft, topRight, bottomLeft, bottomRight;
		
		bb.WeightSensor.GetWeight(total, topLeft, topRight, bottomLeft, bottomRight);
		printf("balance board top left weight: %f\n", topLeft);
		printf("balance board top right weight: %f\n", topRight);
		printf("balance board bottom left weight: %f\n", bottomLeft);
		printf("balance board bottom right weight: %f\n", bottomRight);
		printf("balance board total weight: %f\n", total);
	}
}*/

void HandleStatus(Wii &wm) {
    cout << "battery:" << wm.GetBatteryLevel() << endl;

    //printf("\n");
    //printf("--- CONTROLLER STATUS [wiimote id %i] ---\n\n", wm.GetID());

    //printf("attachment: %i\n", wm.ExpansionDevice.GetType());
    //printf("speaker: %i\n", wm.isUsingSpeaker());
    //printf("ir: %i\n", wm.isUsingIR());
    //printf("leds: %i %i %i %i\n", wm.isLEDSet(1), wm.isLEDSet(2), wm.isLEDSet(3), wm.isLEDSet(4));
    //printf("battery: %f %%\n", wm.GetBatteryLevel());
}

void HandleDisconnect() {
    printf("\n");
    printf("--- DISCONNECTED");
    printf("\n");
}

/*void HandleReadData(CWiimote &wm)
{
    printf("\n");
    printf("--- DATA READ [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void HandleNunchukInserted(CWiimote &wm)
{
    printf("Nunchuk inserted on controller %i.\n", wm.GetID());
}

void HandleClassicInserted(CWiimote &wm)
{
    printf("Classic controler inserted on controller %i.\n", wm.GetID());
}

void HandleGH3Inserted(CWiimote &wm)
{
    printf("GH3 inserted on controller %i.\n", wm.GetID());
}*/

int main(int argc, char** argv) {
    Wii* wii = new Wii();

    while(true) {
        int device_id = hci_get_route(NULL);
        if (device_id < 0) {

        } else {
            break;
        }
        usleep(1000);
    }

    wii->Connect("0C:FC:83:BE:61:02");
    
    wii->Set_Leds(WIIMOTE_LED_2);

	usleep(5000);

    do {
        wii->Pool();
        if(1) {
            Wii::EventTypes event = wii->GetEvent();
            //cout << "\nEvent " << (uint8_t)event << endl;
            switch(event) {
                case Wii::EventTypes::EVENT:
                    HandleEvent(*wii);
                    break;

                case Wii::EventTypes::STATUS:
                    HandleStatus(*wii);
                    break;

                case Wii::EventTypes::DISCONNECT:
                case Wii::EventTypes::UNEXPECTED_DISCONNECT:
                    HandleDisconnect();
                    break;

                default:
                    break;
            }
        }
    } while(wii->Connected());

    return 0;
}
