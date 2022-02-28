# Bang Sound Speaker

Arduino Controled Lights effect sync with the music.

## Sound Sync

In order to sync the music with the light effect is u

### Beat Detection

To react to the sound is used a Mic Module. In order to sync with the music a very basic approach is used. If the sound volume is higher then a treashold then it count as a beat.

## Lights Effects

A addressed light strip is used for all light effects. The box has 2 light strips, one placed in botton of the box and other for all speakers.

Every time a beat is detected the lights change. The color and the effect type is changed using a timer. 

### Base

There are 8 led segments displaced in under the box. The are placed as follow:

Led Position Index:
```
     (Front)
-----------------
| (6)  (7)  (0) |
| (5)       (1) |
| (4)  (3)  (2) |
-----------------
     (Back)
```

### Speakers

The sound box has 4 speakers and each one has 4 led segments. 

The led address is given by $led_i = 4*n + i$ with $n$ as the speaker number and $i$ as the index as follow:

```
      (top)
-----------------
|      (0)      |
| (3)       (1) |
|      (2)      |
-----------------
    (botton)
```
