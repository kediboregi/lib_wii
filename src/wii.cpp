#include "wii.h"
using namespace std;

Wii::Wii() {
	//bdaddr = *BDADDR_ANY;
	out_sock = -1;
	in_sock = -1;	

    state = WIIMOTE_INIT_STATES;
    flags = WIIC_INIT_FLAGS;
    autoreconnect = 0;
    event = EventTypes::NONE;
}

Wii::~Wii() {
	Disconnect();
}

int Wii::Connect(string address) {
	if (Connected())
		return 0;

    struct sockaddr_l2 addr;
	memset(&addr, 0, sizeof(addr));

	addr.l2_family = AF_BLUETOOTH;

	if (address.length()) {
		//int n = address.length();
		//char str[n + 1];
		//strcpy(str, address.c_str());
		str2ba(address.c_str(), &addr.l2_bdaddr);
	} else
		addr.l2_bdaddr = this->bdaddr;

	// nOUTPUT CHANNEL
	out_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (out_sock == -1)
		return 0;

	addr.l2_psm = htobs(WM_OUTPUT_CHANNEL);

	/* connect to wiimote */
	if (connect(out_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("connect() output sock");
		return 0;
	}

	// INPUT CHANNEL
	in_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (in_sock == -1) {
		close(out_sock);
		out_sock = -1;
		return 0;
	}

	addr.l2_psm = htobs(WM_INPUT_CHANNEL);

	/* connect to wiimote */
	if (connect(in_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		close(out_sock);
		out_sock = -1;
		return 0;
	}
	
	/* autoreconnect flag */
	//autoreconnect = autoreconnect;

	/* do the handshake */
	WIIMOTE_ENABLE_STATE(state, WIIMOTE_STATE_CONNECTED);
	WIIMOTE_ENABLE_STATE(state, WIIMOTE_STATE_HANDSHAKE);

	Set_Leds(WIIMOTE_LED_NONE);
	usleep(5000);
	Set_Leds(WIIMOTE_LED_NONE);

	Status();

	WIIMOTE_DISABLE_STATE(state, WIIMOTE_STATE_HANDSHAKE);
	WIIMOTE_ENABLE_STATE(state, WIIMOTE_STATE_HANDSHAKE_COMPLETE);
	handshake_state++;

	//Set_Report_Type();

	return 1;
}

void Wii::Disconnect() {
    if (!Connected())
		return;

	close(out_sock);
	close(in_sock);

	out_sock = -1;
	in_sock = -1;
	event = EventTypes::NONE;

	WIIMOTE_DISABLE_STATE(state, WIIMOTE_STATE_CONNECTED);
	WIIMOTE_DISABLE_STATE(state, WIIMOTE_STATE_HANDSHAKE);
}

int Wii::Send(uint8_t report_type, uint8_t* msg, int len) {
	uint8_t buf[32];
	int rumble = 0;

	buf[0] = WM_SET_REPORT | WM_BT_OUTPUT;
	buf[1] = report_type;

	switch (report_type) {
		case WM_CMD_LED:
		case WM_CMD_RUMBLE:
		case WM_CMD_CTRL_STATUS:
		case WM_CMD_REPORT_TYPE:
		{
			/* Rumble flag for: 0x11, 0x13, 0x14, 0x15, 0x19 or 0x1a */
			if (WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_RUMBLE))
				rumble = 1;
			break;
		}
		default:
			break;
	}

	memcpy(buf+2, msg, len);
	if (rumble) 
		buf[2] |= 0x01;
	else 
		buf[2] &= 0xFE;	// Fix for Wiiuse bug

	//#ifdef WITH_WIIC_DEBUG
	{
		int x = 2;
		/*printf("[DEBUG] (id none) SEND: (%x) %.2x ", buf[0], buf[1]);
		for (; x < len+2; ++x)
			printf("%.2x ", buf[x]);
		printf("\n");*/
	}
	//#endif

	return write(out_sock, buf, len+2);
}

int Wii::Set_Report_Type() {
	uint8_t buf[2];
	int motion, exp, ir;

	if (!Connected())
		return 0;

	buf[0] = (WIIMOTE_IS_FLAG_SET(flags, WIIC_CONTINUOUS) ? 0x04 : 0x00);	/* set to 0x04 for continuous reporting */
	buf[1] = 0x00;

	motion = WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_ACC);
	exp = WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_EXP);
	ir = WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_IR);

	if (motion && ir && exp)	buf[1] = WM_RPT_BTN_ACC_IR_EXP;
	else if (motion && exp)		buf[1] = WM_RPT_BTN_ACC_EXP;
	else if (motion && ir)		buf[1] = WM_RPT_BTN_ACC_IR;
	else if (ir && exp)			buf[1] = WM_RPT_BTN_IR_EXP;
	else if (ir)				buf[1] = WM_RPT_BTN_ACC_IR;
	else if (exp) 				buf[1] = WM_RPT_BTN_EXP; 
	else if (motion)			buf[1] = WM_RPT_BTN_ACC;
	else						buf[1] = WM_RPT_BTN;

	WIIC_DEBUG("Setting report type: 0x%x", buf[1]);

	exp = Send(WM_CMD_REPORT_TYPE, buf, 2);
	if (exp <= 0)
		return exp;

	return buf[1];
}

int Wii::Pool() {
	int evnt = 0;

	int r;
	int i;

	/* only poll it if it is connected */
	if (!Connected())
		return 0;

	event = EventTypes::NONE;

	/* clear out the event buffer */
	memset(event_buf, 0, sizeof(event_buf));

	/* read the pending message into the buffer */
	r = read(in_sock, event_buf, sizeof(event_buf));
	
	if (r == -1) {
		/* error reading data */
		cout << "Error Details" << endl;

		if (errno == ENOTCONN) {
			/* this can happen if the bluetooth dongle is disconnected */
			WIIC_ERROR("Bluetooth appears to be disconnected.  Wiimote unid will be disconnected.");
			Disconnect();
			event = EventTypes::UNEXPECTED_DISCONNECT;
		}

	}
	if (!r) {
		cout << "Disconnect" << endl;

		/* remote disconnect */
		Disconnect();
		evnt = 1;
	}
	
	/* propagate the event */
	Propagate_Event(event_buf[1], event_buf+2);
	evnt += (event != EventTypes::NONE);

	return evnt;
}

void Wii::Propagate_Event(uint8_t _event, uint8_t* msg) {
	lstate.btns = btns;
	//lstate.accel = accel;

	switch (_event) {
		case WM_RPT_BTN:
		{
			// button
			pressed_buttons(msg);
			event = EventTypes::EVENT;
			break;
		}
		case WM_RPT_CTRL_STATUS:
		{
			// controller status
			event_status(msg);

			// don't execute the event callback
			return;
		}
		case WM_RPT_WRITE:
		{
			// write feedback - safe to skip
			break;
		}
		default:
		{
			cout << "default event" << _event << endl;
			return;
		}
	}

	//if (btns || btns != lstate.btns)
	//if (btns_held == btns)
	//	event = EventTypes::EVENT;

	//fprintf(stderr, "[DEBUG] events %x %i %x %x %x %x \n", _event, event, msg, lstate.btns, btns, btns_held);
}

void Wii::pressed_buttons(uint8_t* msg) {
	short now;

	/* convert to big endian */
	now = BIG_ENDIAN_SHORT(*(short*)msg) & WIIMOTE_BUTTON_ALL;

	/* pressed now & were pressed, then held */
	/* 
	 * FIXME - With motion sensing disabled and no other activities
	 * it's impossible to state if a button is held, since no other 
	 * report will be sent.
	 */
	//btns_held = (now & btns);
	
	btns_held = (now & btns);

	/* were pressed or were held & not pressed now, then released */
	btns_released = ((btns | btns_held) & ~now);

	/* buttons pressed now */
	btns = (now);
}

void Wii::event_status(uint8_t* msg) {
	int led[4] = {0};
	int attachment = 0;
	int ir = 0;
	int exp_changed = 0;

	event = EventTypes::STATUS;

	//pressed_buttons(msg);

	/* find what LEDs are lit */
	if (msg[2] & WM_CTRL_STATUS_BYTE1_LED_1)	led[0] = 1;
	if (msg[2] & WM_CTRL_STATUS_BYTE1_LED_2)	led[1] = 1;
	if (msg[2] & WM_CTRL_STATUS_BYTE1_LED_3)	led[2] = 1;
	if (msg[2] & WM_CTRL_STATUS_BYTE1_LED_4)	led[3] = 1;

	// is an attachment connected to the expansion port?
	/*if ((msg[2] & WM_CTRL_STATUS_BYTE1_ATTACHMENT) == WM_CTRL_STATUS_BYTE1_ATTACHMENT)
		attachment = 1;

	// is the speaker enabled?
	if ((msg[2] & WM_CTRL_STATUS_BYTE1_SPEAKER_ENABLED) == WM_CTRL_STATUS_BYTE1_SPEAKER_ENABLED)
		WIIMOTE_ENABLE_STATE(wm, WIIMOTE_STATE_SPEAKER);

	// is IR sensing enabled?
	if ((msg[2] & WM_CTRL_STATUS_BYTE1_IR_ENABLED) == WM_CTRL_STATUS_BYTE1_IR_ENABLED)
		ir = 1;*/

	// find the battery level and normalize between 0 and 1 
	battery_level = (msg[5] / (float)WM_MAX_BATTERY_CODE);

	/* expansion port */
	/*if (attachment && !WIIMOTE_IS_SET(wm, WIIMOTE_STATE_EXP)) {
		handshake_expansion(wm, NULL, 0);
		exp_changed = 1;
	} else if (!attachment && WIIMOTE_IS_SET(wm, WIIMOTE_STATE_EXP)) {
		disable_expansion(wm);
		exp_changed = 1;
	}*/

	Set_Report_Type();
}

Wii::EventTypes Wii::GetEvent() {
    return event;
}

bool Wii::Connected() {
	if (WIIMOTE_IS_CONNECTED(state)) 
		return true;
	return false;
}

void Wii::Set_Leds(int _leds) {
	uint8_t buf;

	if (!Connected()) 
		return;

	/* remove the lower 4 bits because they control rumble */
	leds = (_leds & 0xF0);
	buf = leds;

	Send(WM_CMD_LED, &buf, 1);
}

int Wii::Get_Leds() {
    return leds;
}

void Wii::Status() {
	uint8_t buf = 0;

	if (!Connected())
		return;

	//WIIC_DEBUG("Requested wiimote status.");

	Send(WM_CMD_CTRL_STATUS, &buf, 1);
}

float Wii::GetBatteryLevel() {
    return battery_level;
}

int Wii::GetHandshakeState() {
    return handshake_state;
}

bool Wii::RumbleEnabled() {
	if (WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_RUMBLE)) 
		return true;
	return false;
}

void Wii::Rumble(int status) {
	uint8_t buf;

	if (!Connected())
		return;

	buf = leds;

	if (status) {
		WIIMOTE_ENABLE_STATE(state, WIIMOTE_STATE_RUMBLE);
	} else {
		WIIMOTE_DISABLE_STATE(state, WIIMOTE_STATE_RUMBLE);
	}

	/* preserve IR state */
	if (WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_IR))
		buf |= 0x04;

	Send(WM_CMD_RUMBLE, &buf, 1);
}

void Wii::ToggleRumble() {
	Rumble(!WIIMOTE_IS_STATE_SET(state, WIIMOTE_STATE_RUMBLE));
}