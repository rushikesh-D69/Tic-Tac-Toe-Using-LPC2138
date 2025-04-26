#include <lpc214x.h>

// ---------- Segment Values ----------
#define SEG_0      0x3F // 0 on 7-segment display
#define SEG_H      0x76 // H on 7-segment display
#define SEG_BLANK  0x00 // Blank display

// Forward declarations
void delay_ms(unsigned int ms);
void shift_bit(unsigned int data_pin, unsigned int clk_pin, unsigned char bit_val);
void shift_byte(unsigned int data_pin, unsigned int clk_pin, unsigned char byte_val);
void display_char(unsigned int data_pin, unsigned int clk_pin, unsigned char seg_code);
void setup_display(unsigned int data_pin, unsigned int clk_pin, unsigned int mr_pin);
unsigned char read_button(unsigned int button_pin);
void toggle_display(unsigned int data_pin, unsigned int clk_pin, unsigned char state);

// Define structures for display configuration
typedef struct {
    unsigned int data_pin;
    unsigned int clk_pin;
    unsigned int mr_pin;
} display_config_t;

// Forward declarations for functions using the custom type
void show_winner(unsigned char winner, display_config_t displays[], int num_displays);
void clear_all(unsigned char* states, display_config_t displays[], int num_displays);
unsigned char check_win(unsigned char states[9]);
unsigned char check_draw(unsigned char states[9]);

// ---------- Shift Register Setup ----------
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 6000; j++);
}

void shift_bit(unsigned int data_pin, unsigned int clk_pin, unsigned char bit_val) {
    if (bit_val) IOSET0 = data_pin;
    else IOCLR0 = data_pin;

    IOSET0 = clk_pin;
    delay_ms(1);
    IOCLR0 = clk_pin;
}

void shift_byte(unsigned int data_pin, unsigned int clk_pin, unsigned char byte_val) {
    int i;
    for (i = 7; i >= 0; i--) {
        shift_bit(data_pin, clk_pin, (byte_val >> i) & 1);
    }
}

void display_char(unsigned int data_pin, unsigned int clk_pin, unsigned char seg_code) {
    shift_byte(data_pin, clk_pin, seg_code);
}

void setup_display(unsigned int data_pin, unsigned int clk_pin, unsigned int mr_pin) {
    IODIR0 |= data_pin | clk_pin | mr_pin;
    IOCLR0 = mr_pin;
    delay_ms(1);
    IOSET0 = mr_pin;
    
    // Initialize with blank display
    display_char(data_pin, clk_pin, SEG_BLANK);
}

unsigned char read_button(unsigned int button_pin) {
    if (IOPIN1 & button_pin) {  // Active high check
        delay_ms(20);  // Debounce delay
        if (IOPIN1 & button_pin) return 1;  // Confirm it's still pressed
    }
    return 0;
}


void toggle_display(unsigned int data_pin, unsigned int clk_pin, unsigned char state) {
    if (state == 0) {
        display_char(data_pin, clk_pin, SEG_BLANK);
    } else if (state == 1) {
        display_char(data_pin, clk_pin, SEG_H);  // Display 'H'
    } else if (state == 2) {
        display_char(data_pin, clk_pin, SEG_0);  // Display '0'
    }
}

void show_winner(unsigned char winner, display_config_t displays[], int num_displays) {
    unsigned char seg = (winner == 1) ? SEG_H : SEG_0;
    int i;
    
    // Display the winner across all displays
    for (i = 0; i < num_displays; i++) {
        display_char(displays[i].data_pin, displays[i].clk_pin, seg);
    }
}

void clear_all(unsigned char* states, display_config_t displays[], int num_displays) {
    int i;
    for (i = 0; i < 9; i++) states[i] = 0;
    
    for (i = 0; i < num_displays; i++) {
        display_char(displays[i].data_pin, displays[i].clk_pin, SEG_BLANK);
    }
}

unsigned char check_win(unsigned char states[9]) {
    // Check for win in rows, columns, and diagonals
    if (states[0] && states[0] == states[1] && states[1] == states[2]) return states[0];
    if (states[3] && states[3] == states[4] && states[4] == states[5]) return states[3];
    if (states[6] && states[6] == states[7] && states[7] == states[8]) return states[6];

    if (states[0] && states[0] == states[3] && states[3] == states[6]) return states[0];
    if (states[1] && states[1] == states[4] && states[4] == states[7]) return states[1];
    if (states[2] && states[2] == states[5] && states[5] == states[8]) return states[2];

    if (states[0] && states[0] == states[4] && states[4] == states[8]) return states[0];
    if (states[2] && states[2] == states[4] && states[4] == states[6]) return states[2];

    return 0;  // No winner
}

unsigned char check_draw(unsigned char states[9]) {
    int i;
    for (i = 0; i < 9; i++) {
        if (states[i] == 0) return 0; // Game still ongoing
    }
    return 1; // All cells filled, it's a draw
}

int main(void) {
    unsigned char state[9] = {0};  // Store the state of each cell (0 = blank, 1 = H, 2 = 0)
    unsigned char winner = 0;
    unsigned char current_player = 1;  // 1 for Player H, 2 for Player 0
    unsigned char is_draw = 0;
    int i, j, k;
    unsigned int timeout;
    
    // Configuration for the 9 displays as specified in your code
    display_config_t displays[9] = {
        {1<<0, 1<<1, 1<<2},    // U2
        {1<<3, 1<<4, 1<<5},    // U3
        {1<<6, 1<<7, 1<<8},    // U4
        {1<<9, 1<<10, 1<<11},  // U5
        {1<<12, 1<<13, 1<<14}, // U6
        {1<<15, 1<<16, 1<<17}, // U7
        {1<<18, 1<<19, 1<<20}, // U8
        {1<<21, 1<<22, 1<<23}, // U9
        {1<<25, 1<<26, 1<<27}  // U10
    };
    
    // Button pins as specified in the original code
    unsigned int button_pins[9] = {
        1<<16,  // Button for U2
        1<<17,  // Button for U3
        1<<18,  // Button for U4
        1<<19,  // Button for U5
        1<<20,  // Button for U6
        1<<21,  // Button for U7
        1<<22,  // Button for U8
        1<<23,  // Button for U9
        1<<24   // Button for U10
    };
    
    // Reset button pin
    unsigned int reset_button_pin = 1<<25;
    
    // Configure button pins as inputs
    IODIR1 &= ~((1<<16)|(1<<17)|(1<<18)|(1<<19)|(1<<20)|(1<<21)|(1<<22)|(1<<23)|(1<<24)|(1<<25));
    
    // Initialize displays
    for (i = 0; i < 9; i++) {
        setup_display(displays[i].data_pin, displays[i].clk_pin, displays[i].mr_pin);
    }

    while (1) {
        if (winner == 0 && is_draw == 0) {  // Game continues until there's a winner or draw
            // Check button presses for each cell
            for (i = 0; i < 9; i++) {
                if (read_button(button_pins[i]) && state[i] == 0) {
                    state[i] = current_player;
                    toggle_display(displays[i].data_pin, displays[i].clk_pin, current_player);
                    
                    // Wait for button release with timeout
                    timeout = 1000; // 1 second timeout
                    while (!(IOPIN1 & button_pins[i]) && timeout > 0) {
                        delay_ms(1);
                        timeout--;
                    }
                    
                    // Check for winner after each move
                    winner = check_win(state);
                    if (winner) {
                        show_winner(winner, displays, 9);
                        break; // Exit the for loop once we have a winner
                    }
                    
                    // Check for draw
                    is_draw = check_draw(state);
                    if (is_draw) {
                        // Flash all displays to indicate draw
                        for (j = 0; j < 3; j++) { // Flash 3 times
                            for (k = 0; k < 9; k++) {
                                display_char(displays[k].data_pin, displays[k].clk_pin, SEG_BLANK);
                            }
                            delay_ms(500);
                            for (k = 0; k < 9; k++) {
                                display_char(displays[k].data_pin, displays[k].clk_pin, 
                                            (state[k] == 1) ? SEG_H : ((state[k] == 2) ? SEG_0 : SEG_BLANK));
                            }
                            delay_ms(500);
                        }
                        break; // Exit the for loop if it's a draw
                    }
                    
                    // Switch to the other player only after a successful move
                    current_player = (current_player == 1) ? 2 : 1;
                    break; // Exit the for loop after a successful move
                }
            }
        }

        // Check for reset button press
        if (read_button(reset_button_pin)) {
            clear_all(state, displays, 9);  // Clear the game state
            winner = 0;         // Reset winner
            is_draw = 0;        // Reset draw status
            current_player = 1; // Player H starts first again
            
            // Wait for reset button release with timeout
            timeout = 1000; // 1 second timeout
            while (!(IOPIN1 & reset_button_pin) && timeout > 0) {
                delay_ms(1);
                timeout--;
            }
        }
    }
}