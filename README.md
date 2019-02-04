# Inertia Wheel Inverted Pendulum Project 

[![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master)](https://travis-ci.org/joemccann/dillinger)

This repo contains CubeMX and Keil project for Inverted pendulum project based on Nucleo STM32F411RE microcontroller. 
This project is suitable for electrical engineering studets which want a unstable proces to test their algorithms, the primary contorller used in this project is LQR.  

The video of it working can be seen on link:
[![IMAGE ALT TEXT HERE](https://lh3.googleusercontent.com/lRLYiF8R0qKQ-6XbWDhnF0ymqUXd8Q8ov5O4GAzWkT10UBAVR2bPio0WnbaPWFgZ04aVPHDtyS5m6trpa7smPcwIV6ihfHjh1vP1wBdLZrUop1bMNOYDJiz07E4lLRaJ4KR5n7zTrhrJVVBgNj-Swd1UN3rdiENvCENmuJnUysM-51tJMC9GnquH1crIs4L46KtmS5Zb7fAyklV2Mhpx1zqIFVQgmqkXOYG7bQevg3HC5PyYHTU6NSA6cS_oyk9JkTgw5VqoY1sERGsMP4bQdE3Kqbn-AmWLY5FX3l2Qy-2a8j7lPxdA5RrWelHTLkEra1usqx-0Y_T9ffiAvAvYujnpWNL7IQQeFnxy0A8RrDpTz9nLiu5wb4A0tU-JzkAmWOFcuVNelB0R5kTJAiLrIffieNZqNJ3VGT60pph-XM2IbLhTWgZQbySeJPdLOHva8Rf8RuToQVAJNqIrjKrSYFuuDrgtcOzGSZHmb71BujSQMRQLaI3uh9dJqZdsMNDrO-0cgs_MVScM2I3vxzBK0X4BZEBfaKturiXOEEDtjBmW6FzLh_5xK-YhANGaW_Rz60VrcHxqJfg-fqz7Cx1BySHdEnWp1COnvA7N0hszqccVJru6K09_Cd3dL9S31l4IXWGA6JtuB4LY6hG_l1s_VKzfEqqjnV2e=w534-h949-no)](https://photos.app.goo.gl/MYDtRjFzWWU6QyZS6)


# Electronics hardware
- Nuceleo STM32F411RE - [Mouser](https://hr.mouser.com/ProductDetail/STMicroelectronics/NUCLEO-F411RE?qs=%2fha2pyFaduj0LE%252bzmDN2WGOyc9ZJn6rAAaZxUZnl1jWj%2fTN4JYE9Mw%3d%3d) 
- Monster motor shield - [Ebay](https://www.ebay.com/itm/L298N-Dual-VNH2SP30-Stepper-Motor-Driver-Module-30A-Monster-Moto-Shield-Replace/112031018900?hash=item1a1591af94:g:R4YAAOSwEaBaTafh)
- Motor - Mabuchi RS-385PH with encoder (hall or optical)
  -  With encoder or hall sensors  [Ebay](https://www.ebay.com/itm/RS-385PH-16140-MABUCHI-DC-24V-Motor-for-Printer-Copy-machine/222827327725?epid=24014452613&hash=item33e18b30ed:g:ulEAAOSwZFdaecSZ:rk:8:pf:0)
  -  I used motor without sensor and retrofited it with Hall encoder 20cpr [Ebay](https://www.ebay.com/itm/Pololu-Romi-Encoder-Pair-Kit-12-CPR-3-5-18V-with-Hall-effect-sensor-3542/272789024914?epid=2157895482&hash=item3f837e3092:g:MxkAAOSwu9Jb3CQt:rk:1:pf:1&frcectupt=true)
- Encoder [Aliexpress](https://www.aliexpress.com/item/360-600P-R-Photoelectric-Incremental-Rotary-Encoder-5-24V-AB-Two-Phases-Shaft/32756209569.html?spm=2114.search0104.3.38.1c0b9456Bf5oQP&ws_ab_test=searchweb0_0,searchweb201602_1_10065_10068_319_10059_10884_317_10887_10696_321_322_453_10084_454_10083_10103_10618_10307_10820_10301_10821_10303_537_536_10902,searchweb201603_45,ppcSwitch_0&algo_expid=3179dab4-6711-42f4-8e94-b644142c4211-5&algo_pvid=3179dab4-6711-42f4-8e94-b644142c4211)

# 3D printed parts
 - bla bla

# Control algorithm
 - LQR algorithm based parametered for using precise model
 
