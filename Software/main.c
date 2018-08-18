/**
 *
 * @file main.c
 *
 * @date May 24, 2017
 * @author hamster
 *
 * @brief This is the entry point for the DC801 DC26 party badge
 *
 */

#include <stdint.h>
#include "common.h"
#include "user.h"

volatile bool partyMode = false;
volatile bool sheepMode = false;

APP_TIMER_DEF(standby_animation_timer_id);

static void standby_animation_timeout_handler(void *p_context) {
    UNUSED_PARAMETER(p_context);

    util_gfx_draw_raw_file_stop();
}

/**
 * Initialize the LEDs
 *
 * LED_POWER_UP_2 is not used because there is some bug in the softdevice where it realllllly
 * wants to flash that LED on LCD SPI access and it's just annoying.  So, we'll just disabled.
 */
static void led_init(void){
    // Setup and turn off the LEDs
    nrf_gpio_cfg_output(LED_LEVEL_UP_0);
    nrf_gpio_cfg_output(LED_LEVEL_UP_1);
    nrf_gpio_cfg_output(LED_LEVEL_UP_2);
    nrf_gpio_cfg_output(LED_LEVEL_UP_3);
    nrf_gpio_cfg_output(LED_POWER_UP_0);
    nrf_gpio_cfg_output(LED_POWER_UP_1);
    //nrf_gpio_cfg_output(LED_POWER_UP_2);
    nrf_gpio_cfg_output(LED_POWER_UP_3);

    nrf_gpio_pin_set(LED_LEVEL_UP_0);
    nrf_gpio_pin_set(LED_LEVEL_UP_1);
    nrf_gpio_pin_set(LED_LEVEL_UP_2);
    nrf_gpio_pin_set(LED_LEVEL_UP_3);
    nrf_gpio_pin_set(LED_POWER_UP_0);
    nrf_gpio_pin_set(LED_POWER_UP_1);
    nrf_gpio_pin_set(LED_POWER_UP_2);
    nrf_gpio_pin_set(LED_POWER_UP_3);
}

/**
 * Initialize the buttons
 */
static void button_init(void){
    // Setup the buttons
    nrf_gpio_cfg_input(USER_BUTTON_UP, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_DOWN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_LEFT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_RIGHT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_A, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_B, NRF_GPIO_PIN_NOPULL);
}

/**
 * Initialize the speaker
 */
static void speaker_init(void){
    // Setup the beeper
    nrf_gpio_cfg_output(SPEAKER);
}

/**
 * Initialize the logging backend for logging over JTAG
 */
static void log_init(void){
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * Handler to show the LEDs during bootup animation
 * @param frame
 * @param p_data
 */
static void bootCallback(uint8_t frame, void *p_data){

    if(frame > 50){
        nrf_gpio_pin_clear(LED_LEVEL_UP_0);
    }
    if(frame > 100){
        nrf_gpio_pin_clear(LED_LEVEL_UP_1);
    }
    if(frame > 150){
        nrf_gpio_pin_clear(LED_LEVEL_UP_2);
    }
    if(frame > 200){
        nrf_gpio_pin_clear(LED_LEVEL_UP_3);
    }

    if(frame > 205){
        nrf_gpio_pin_clear(LED_POWER_UP_0);
    }
    if(frame > 210){
        nrf_gpio_pin_clear(LED_POWER_UP_1);
    }
    if(frame > 215){
        nrf_gpio_pin_clear(LED_POWER_UP_2);
    }
    if(frame > 222){
        nrf_gpio_pin_clear(LED_POWER_UP_3);
    }

}

/**
 * Handler to show the LED blink during party mode
 * @param frame
 * @param p_data
 */
static void partyCallback(uint8_t frame, void *p_data){

    if(frame % 8 == 0){
        setLevelLEDs(LEVEL4);
    }
    else{
        setLevelLEDs(LEVEL0);
    }

    if(frame % 2 == 0){
        setPowerUpLEDs(POWERUP_0);
        nrf_gpio_pin_clear(LED_POWER_UP_0);
    }
    if(frame % 3 == 0){
        setPowerUpLEDs(POWERUP_0);
        nrf_gpio_pin_clear(LED_POWER_UP_1);
    }
    if(frame % 4 == 0){
        setPowerUpLEDs(POWERUP_0);
        nrf_gpio_pin_clear(LED_POWER_UP_3);
    }
    if(frame == 0){
        setPowerUpLEDs(POWERUP_0);
    }


}

typedef enum {
    item_games,
    item_nearby,
    item_extras,
    item_credits,
    item_godmode,
    NUM_MENU_MAIN_ITEMS
} MENU_MAIN;


MENU mainMenu[NUM_MENU_MAIN_ITEMS] = {
        { item_games, "Games" },
        { item_nearby, "Nearby" },
        { item_extras, "Extras"},
        { item_credits, "Credits" },
        { item_godmode, "God Mode"}
};

/**
 * @brief Main app
 * @return Not used
 */
int main(void){

    // Setup the system
    led_init();
    button_init();
    speaker_init();
    log_init();

    // Pop pop!
    beep(75, 600);
    nrf_delay_ms(50);
    beep(150, 800);
    nrf_delay_ms(50);

    // Timers
    app_timer_init();

    // BLE
    //gap_params_init();
    ble_stack_init();
    scan_start();

    // Init the display
    st7735_init();
	st7735_start();
	util_gfx_init();

    if(!util_sd_init()){
        util_sd_error();
    }

    // Init the random number generator
    nrf_drv_rng_init(NULL);

    // Setup the battery monitor
    adc_configure();
    adc_start();

    // Setup the UART
    uart_init();

    // Boot! Boot! Boot!
    printf("Booted!\n");
    // printf goes to the RTT_Terminal.log after you've fired up debug.sh

    util_gfx_draw_raw_file("BOOT.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, bootCallback, false, NULL);

    setPowerUpLEDs(POWERUP_0);
    setLevelLEDs(LEVEL0);

    // Load the user
    loadUser();

    if(!user.configured){
        // Configure the system
        printf("First time setup\n");
        firstSetup();
    }

    // Init the WarGames game
    wg_Init();

    // Configure the systick
    sysTickStart();

    // Setup a timer for shutting down animations in standby
    app_timer_create(&standby_animation_timer_id, APP_TIMER_MODE_SINGLE_SHOT, standby_animation_timeout_handler);

    // Setup the BLE
    advertising_setUser(user.name);
    advertising_setClan(user.clan);
    advertising_setScore(user.score);

    ble_adv_start();

    setTempScoreModifier(0);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {

        util_gfx_fill_screen(COLOR_BLACK);

        drawScreenTemplate();

        // Draw the main menu

#ifdef GODMODE
        int subMenu = getMenuSelection(mainMenu, 50, ARRAY_SIZE(mainMenu), 4, 15000, true);
#else
        int subMenu = getMenuSelection(mainMenu, 50, ARRAY_SIZE(mainMenu) - 1, 4, 15000, true);
#endif

        switch(subMenu){
            case item_games:
                games();
                break;
            case item_nearby:
                nearby();
                break;
            case item_extras:
                extras();
                break;
            case item_credits:
                credits();
                break;
            case item_godmode:
                godMode();
                break;
            default:
                // Show the standby screen
                showStandby();
                break;
        }

    }
#pragma clang diagnostic pop

}

/**
 * Show the standby screen
 * @param p_context
 */
void showStandby(void){

    uint16_t nearbyCounter = 0;
    uint16_t animationCounter = 0;
    uint8_t delayCount = 0;
    drawStandby();

    while(true){
        if(getButton(false)){
            // User pressed a button
            return;
        }
        nrf_delay_ms(25);

        if(partyMode){
            // PARTYYYY!
            util_gfx_draw_raw_file("/EXTRAS/SHEEP6.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, partyCallback, true, NULL);
        }

        if(sheepMode){
            // SHEEEEP
            do{
                uint8_t i;
                util_gfx_draw_raw_file("/EXTRAS/SHEEP1.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
                util_gfx_draw_raw_file("/EXTRAS/SHEEP2.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
                util_gfx_draw_raw_file("/EXTRAS/SHEEP3.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
                util_gfx_draw_raw_file("/EXTRAS/SHEEP4.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
                util_gfx_draw_raw_file("/EXTRAS/SHEEP5.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
                util_gfx_draw_raw_file("/EXTRAS/SHEEP6.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                for(i = 0; i < 100; i++){
                    if(getButton(false)){
                        break;
                    }
                    nrf_delay_ms(25);
                }
            }while (!getButton(false));

        }

        if(animationCounter ++ > (15000 / 25)){
            // Show an animation

            uint8_t rand;
            nrf_drv_rng_rand(&rand, 1);

            char files[64][9];
            uint8_t numFiles = 0;

            memset(files, 0, sizeof(files));

            numFiles = getFiles(files, "/EXTRAS", 64);

            rand = rand % numFiles;

            app_timer_start(standby_animation_timer_id, APP_TIMER_TICKS(10000), NULL);

            char filename[21];
            snprintf(filename, 21, "/EXTRAS/%s.RAW", files[rand]);

            util_gfx_draw_raw_file(filename, 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, true, NULL);

            animationCounter = 0;

            drawStandby();

        }

        if(nearbyCounter++ > (5000 / 25)){
            // Draw a shout
            uint8_t rand;
            nrf_drv_rng_rand(&rand, 1);

            bool haveBadge = false;
            BADGE_ADV badge;
            rand = rand % getBadgeNum();

            if(getBadge(rand, &badge)){

                if(getBadgeIconFile(badge.group) != NULL) {
                    util_gfx_draw_raw_file(getBadgeIconFile(badge.group), 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);
                }
                else{
                    util_gfx_draw_raw_file("/GROUPS/MISSING.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);

                    util_gfx_fill_rect(0, 0, GFX_WIDTH, 15, COLOR_BLACK);

                    uint16_t w, h;
                    char name[16];
                    util_gfx_set_font(FONT_VERAMONO_5PT);
                    util_gfx_get_text_bounds(getBadgeGroupName(badge.group), 0, 0, &w, &h);
                    util_gfx_set_cursor(64 - (w / 2), 2);

                    snprintf(name, 16, "%s", getBadgeGroupName(badge.group));
                    util_gfx_print(name);
                }

                for(int i = 0; i < 10; i++){
                    if(i % 2 == 0) {
                        setPowerUpLEDs(POWERUP_4);
                        setLevelLEDs(LEVEL0);
                    }
                    else{
                        setPowerUpLEDs(POWERUP_0);
                        setLevelLEDs(LEVEL4);
                    }

                    delayCount = 0;
                    while(delayCount++ < 10) {
                        if(getButton(false)){
                            setPowerUpLEDs(POWERUP_0);
                            setLevelLEDs(LEVEL0);
                            return;
                        }
                        nrf_delay_ms(25);
                    }
                }
                setPowerUpLEDs(POWERUP_0);
                setLevelLEDs(LEVEL0);

                delayCount = 0;
                while(delayCount++ < 40) {
                    if(getButton(false)){
                        return;
                    }
                    nrf_delay_ms(25);
                }

                drawStandby();
            }

            nearbyCounter = 0;

        }
    }

}

/**
 * Draw the user stats on the standby screen
 */
void drawStandby(void){

    // Setup the main screen
    util_gfx_fill_screen(COLOR_BLACK);

    // Show the username
    util_gfx_set_font(FONT_MONO55_8PT);
    util_gfx_set_color(COLOR_WHITE);

    uint16_t w, h;
    util_gfx_get_text_bounds(user.name, 0, 0, &w, &h);
    util_gfx_set_cursor(64 - (w / 2), 2);
    util_gfx_print(user.name);

    util_gfx_set_font(FONT_GAMEPLAY_5PT);
    util_gfx_set_color(COLOR_BLUE);
    util_gfx_set_cursor(2, 20);

    char score[11];
    snprintf(score, 11, "%u", user.score);
    util_gfx_print(score);

    char modifier[6];
    if(getTotalScoreModifier() > 0){
        util_gfx_set_color(COLOR_GREEN);
    }
    if(getTotalScoreModifier() == 0){
        util_gfx_set_color(COLOR_WHITE);
    }
    if(getTotalScoreModifier() < 0){
        util_gfx_set_color(COLOR_RED);
    }
    snprintf(modifier, 6, "%+d", getTotalScoreModifier());
    util_gfx_get_text_bounds(modifier, 0, 0, &w, &h);
    util_gfx_set_cursor(GFX_WIDTH - w - 1, 20);
    util_gfx_print(modifier);

    util_gfx_draw_raw_file(getClanFile(user.clan), 0, 29, GFX_WIDTH, 100, NULL, false, NULL);

}


/**
 * Configure a new user
 */
void firstSetup(void){

    // Clear out the user data
    memset(&user, 0, sizeof(USER));

    util_gfx_fill_screen(COLOR_BLACK);
    util_gfx_set_font(FONT_COMPUTER_12PT);
    util_gfx_set_color(COLOR_BLUE);
    util_gfx_set_cursor(20, 0);
    util_gfx_print("Welcome");

    util_gfx_set_font(FONT_MONO55_8PT);
    util_gfx_set_color(COLOR_WHITE);
    util_gfx_set_cursor(5, 20);
    util_gfx_print("Can you get the\n highest score?");


    util_gfx_set_cursor(28, 65);
    util_gfx_print("Let's get\n    started");

    util_gfx_set_cursor(35, 110);
    util_gfx_print("Press A");

    while(getButton(false) != USER_BUTTON_A){
        // Wait until pressed
    }
    while(getButton(false) == USER_BUTTON_A){
        // Wait until released
    }

    util_gfx_fill_rect(0, 18, 128, 88, COLOR_BLACK);
    util_gfx_set_cursor(8, 20);
    util_gfx_print("You can change\n later and your\n score will be\n     kept");


    while(getButton(false) != USER_BUTTON_A){
        // Wait until pressed
    }
    while(getButton(false) == USER_BUTTON_A){
        // Wait until released
    }

    util_gfx_fill_rect(0, 18, 128, 88, COLOR_BLACK);
    util_gfx_set_cursor(18, 20);
    util_gfx_print("You play for\n  yourself and\n   your clan");


    while(getButton(false) != USER_BUTTON_A){
        // Wait until pressed
    }
    while(getButton(false) == USER_BUTTON_A){
        // Wait until released
    }

    util_gfx_fill_rect(0, 18, 128, 88, COLOR_BLACK);
    util_gfx_set_cursor(5, 20);
    util_gfx_print("Scoreboard will\n   be at the\n     party");

    while(getButton(false) != USER_BUTTON_A){
        // Wait until pressed
    }
    while(getButton(false) == USER_BUTTON_A){
        // Wait until released
    }


    userConfigure();

}


typedef enum {
    game_invaders,
    game_snake,
    game_pips_the_et,
    game_tic_tac_toe,
    NUM_MENU_GAMES_ITEMS
} MENU_GAMES;


MENU gameMenu[NUM_MENU_GAMES_ITEMS] = {
        { game_invaders, "Invaders" },
        { game_snake, "Snake" },
        { game_pips_the_et, "Pips the ET" },
        { game_tic_tac_toe, "TicTacToe" }
};


/**
 * Show the games menu
 */
void games(void){

    drawScreenTemplate();

    int retScore = 0;
    int getGame;
    if(user.wargameUnlock) {
        getGame = getMenuSelection(gameMenu, 50, ARRAY_SIZE(gameMenu), 4, 15000, true);
    }
    else{
        getGame = getMenuSelection(gameMenu, 50, ARRAY_SIZE(gameMenu) , 4, 15000, true);

    }

    switch(getGame){
        case game_invaders:
            // Space invaders
            retScore = SpaceInvaders();
            break;
        case game_snake:
            // Snake run
            retScore = Snake();
            break;
        case game_pips_the_et:
            retScore = PipsTheET();
            break;
        case game_tic_tac_toe:
            retScore = TicTacToe();
            break;
        default:
            break;
    }


    // Figure out the score modifier
    retScore = retScore + (retScore * ((getTotalScoreModifier() / 100)));

    // Calculate the user's score
    if(user.score + retScore > 0){
        user.score += retScore;
    }
    else{
        user.score = 0;
    }

    storeUser();
    advertising_setScore(user.score);

}




typedef enum {
    extra_reconfig,
    extra_boot,
    extra_groups,
    extra_fun,
    extra_version,
    NUM_MENU_EXTRA_ITEMS
} MENU_EXTRA;


MENU extraMenu[NUM_MENU_EXTRA_ITEMS] = {
        { extra_reconfig, "Reconfig" },
        { extra_boot, "Boot Logo" },
        { extra_groups, "DC Groups" },
        { extra_fun, "Fun Stuff" },
        { extra_version, "Version" }
};

/**
 * Show the extras menu
 */
void extras(void){

    drawScreenTemplate();

    int getExtra = getMenuSelection(extraMenu, 50, ARRAY_SIZE(extraMenu), 4, 15000, true);

    switch(getExtra){
        case extra_reconfig:
            util_gfx_set_font(FONT_MONO55_8PT);
            util_gfx_set_color(COLOR_WHITE);
            userConfigure();
            break;
        case extra_boot:
            util_gfx_draw_raw_file("BOOT.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, bootCallback, false, NULL);
            setPowerUpLEDs(POWERUP_0);
            setLevelLEDs(LEVEL0);
            break;
        case extra_groups:
            extraShowGroups();
            break;
        case extra_fun:
            extraFunBrowser();
            break;
        case extra_version:
            extraVersion();
            break;
        default:
            break;
    }

}

/**
 * Show the credits
 */
void credits(void){

    util_gfx_draw_raw_file("CREDITS.RAW", 0, 0, GFX_WIDTH, GFX_HEIGHT, NULL, false, NULL);

    while(true){
        if(getButton(false)){
            // User pressed a button
            return;
        }
        nrf_delay_ms(10);
    }

}
