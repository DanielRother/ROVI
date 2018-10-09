void initPCNT() {
  pcnt_config_t r_enc_config = {
  GPIO_NUM_12, // Pulse input gpio_num, if you want to use gpio16, pulse_gpio_num = 16, a negative value will be ignored
  GPIO_NUM_13, // Control signal input gpio_num, a negative value will be ignored
  PCNT_MODE_KEEP, // PCNT low control mode
  PCNT_MODE_REVERSE, // PCNT high control mode
  PCNT_COUNT_INC, // PCNT positive edge count mode
  PCNT_COUNT_DIS, // PCNT negative edge count mode
  INT16_MAX, // Maximum counter value
  INT16_MIN, // Minimum counter value
  PCNT_UNIT_0, // PCNT unit number
  PCNT_CHANNEL_0, // the PCNT channel
  };
 
  int16_t r_enc_count;
  pcnt_unit_config(&r_enc_config);

  pcnt_set_filter_value(PCNT_UNIT_0, 250);  // Filter Runt Pulses
  pcnt_filter_enable(PCNT_UNIT_0);

  gpio_set_direction(GPIO_NUM_25,GPIO_MODE_INPUT);
  gpio_pullup_en(GPIO_NUM_25); // Rotary Encoder Button

  gpio_pulldown_en(GPIO_NUM_32);
  gpio_pulldown_en(GPIO_NUM_33);

  pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
  pcnt_counter_clear(PCNT_UNIT_0);
  pcnt_counter_resume(PCNT_UNIT_0);


  while (1) {
   pcnt_get_counter_value(PCNT_UNIT_0, &r_enc_count);
   ESP_LOGI("counter", "%d - button:%d", r_enc_count,  gpio_get_level(GPIO_NUM_25));
    
    Serial.print("Counter value PCNT: ");
    Serial.println(r_enc_count, DEC);

   vTaskDelay(200 / portTICK_PERIOD_MS); // Delay 1000msec & yield.
  }
}