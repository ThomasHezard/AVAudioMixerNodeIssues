# AVAudioMixerNode sample rate conversion issue : demo project

This project aims at demonstrating issues with AVAudioMixerNode sample rate conversion capabilities.

As a reminder, AVAudioMixerNode is supposed to work with inputs and outputs of any sample rate and take care of the sample rate conversions. As we will show here this feature seems to have issues.


****

Three test cases are provided for comparisons. All test cases contain the same demo project, only a few parameters are changed between them. If you want to change the parameters yourself, you can do so in file `AudioManager.m`, where they are defined as constants on top of the file.

All the behaviours described below have been confirmed on the following devices:
- iPad Air 2, iOS 11.3
- iPad Air 2, iOS 12.1.1
- iPad Pro 12.9", iOS 11.3
- iPad Pro 11", iOS 12.1.1
- iPhone 5S, iOS 11.3
- iPhone 6, iOS 12.0
- various simulators with iOS 11.3, 12.0 and 12.1

with Xcode 10.1, compiling with latest iOS SDK (12.1) and the deployement target set to iOS 9.0.


****

The demo project has been inspired by real-life use cases. As it has been mentionned on Apple Developer's forums, AVAudioMixerNode is a handy tool to convert the audio input's data format and especially sample rate. This is usefull in two cases:
- when the project include some custom AUAudioUnit with limited sample rate compatibilities,
- when changing the sample rate of the AUAudioUnits in live (when the user connects a bluetooth headset or a USB audio device for example) without impacting the internal audio states and memories of the AUAudioUnits, is an issue.

We've encountered issues with this feature, as it will be detailed in the three tests presented here.


****

The demo project consists of the following simple audio setup :
```
|------------|      |---------|     |-------|
| INPUT NODE |----->| MIXER 1 |---->|       |
|------------|      |---------|     |       |     |-------------|
                                    | MAIN  |---->| OUTPUT NODE |
          |-------------------|     | MIXER |     |-------------|
          | AUDIO PLAYER NODE |---->|       |
          |-------------------|     |-------|
```

* AudioSession is set to `PlayAndRecord` category, with `DefaultToSpeaker` and `AllowBluetooth` options. Prefered buffer duraiton is set to 5 ms. This corresponds to a classic setup for an interfactive musical apps. The sample rate being the issue here, it will be mentionned in the test cases descriptions.

* Interruptions, configuration changes and media service reset are not dealt with in this project for simplicity, as they have nothing to do with the problem illustrated here. Please restart the demo apps if any of these happen.

* The input is permanently linked to the output through Mixer 1 and Main Mixer. Consequently, we advise to test this project with headphones only in order to avoid audio feedbacks.

* The audio player can be controlled by three buttons in main interface: play, pause and stop. An audio file is provided in the project for testing purpose.

* The main interface provides a `Log` button, which triggers the logging, in the terminal, of all the connections' format (also automatically done at startup).


****

## TEST 01

This test illustrates the expected behaviour.

Sample rates for the audio session as well as for all the connections are set to the standard 48 kHz :
```
 48kHz |------------| 48kHz |---------| 48kHz |-------|
------>| INPUT NODE |------>| MIXER 1 |------>|       |
       |------------|       |---------|       |       | 48kHz |-------------| 48kHz
                                              | MAIN  |------>| OUTPUT NODE |------>
                  |-------------------| 48kHz | MIXER |       |-------------|
                  | AUDIO PLAYER NODE |------>|       |
                  |-------------------|       |-------|
```
The sample rate in input of the input node and in output of the output node correspond to the hardware sample rate.

Running this test allow the user to listen to the audio input in direct and play/pause/stop the audio file at the same time.


****

## TEST 02

Here, the audio session ("hardware") sample rate is set to 44.1kHz while the connections inside the graph are set to 48kHz.
This happens typically when the user uses bluetooth audio devices and your app uses code that runs only at 48kHz, or AUAudioUnits that cannot change their sample rate during the app lifecycle.

Note that the input node has the same sample rate at input and output, as it has been stated the AVAudioInputNode does not provide sample rate conversion.
```
 44.1k |------------| 44.1k |---------| 48kHz |-------|
------>| INPUT NODE |------>| MIXER 1 |------>|       |
       |------------|       |---------|       |       | 44.1k |-------------| 44.1k
                                              | MAIN  |------>| OUTPUT NODE |------>
                  |-------------------| 48kHz | MIXER |       |-------------|
                  | AUDIO PLAYER NODE |------>|       |
                  |-------------------|       |-------|
```
We expect the behaviour to be exactly the same, as AVAudioMixerNodes are supposed to take care of sample rate conversions. However, what happens is the following:

* When you start the app, the direct audio feedback does not work: you do not hear the input in the output.
* When you start the audio player, the music starts, and the direct audio feedback starts working: you can hear both the music and the audio input from the mircophone.
* From there, the direct audio feedback will keep working, even if you pause or stop the player.


****

## TEST 03

Same as test 02, but here the the connection between mixer 1 and main mixer uses the "hardware" sample rate.
```
 44.1k |------------| 44.1k |---------| 44.1k |-------|
------>| INPUT NODE |------>| MIXER 1 |------>|       |
       |------------|       |---------|       |       | 44.1k |-------------| 44.1k
                                              | MAIN  |------>| OUTPUT NODE |------>
                  |-------------------| 48kHz | MIXER |       |-------------|
                  | AUDIO PLAYER NODE |------>|       |
                  |-------------------|       |-------|
```
Everything works fine in this case, and we obtain the same excepted behaviour as in TEST 01.


****

## Additional observations are remarks

* The issue in test 02 doesn't appear for all values of the sample rates. Especially, it seems that the problem doesn't occur when the processing sample rate is a mulitple of the hardware sample rate, 16kHz / 48kHz or 24kHz / 48kHz for example.

* The issue is the same even if we add on or several additional mixer between Mixer 1 and Main Mixer.

* Test 02 was impossible to reproduce on iPhone X, as the device refuses to set the audio session's sample rate to 44.1kHz. However, switching the two sample rate (48kHz as hardware sample rate and 44.1kHz as processing sample rate) in that led to the reproduction of the behaviour on iPhone X.

* This is a big issue as some applications only include input-to-output audio graph (with aditionnal effects nodes) and no additional player or generator nodes. In this type of setup, there seems to be no solution to trigger the input processing and the input never finds its way to the output.
