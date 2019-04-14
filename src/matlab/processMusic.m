%% Music Processing script
%
% @author Rafael Lima

% Load Wav Music
musicFileName = 'Daft Punk - The Brainwasher.wav'
%musicFileName = 'Daft Punk - One More Time.wav'
musicInfo = audioinfo(musicFileName)
music = audioread(musicFileName);
musicTime = (1:musicInfo.TotalSamples)/musicInfo.SampleRate;

% play music
sound(music,musicInfo.SampleRate,musicInfo.BitsPerSample)
% stop music
clear sound

% Drums
passLowFreq = 300
lpFilt = designfilt('lowpassiir','FilterOrder',4, ...
        'PassbandFrequency',passLowFreq,'PassbandRipple',0.2, ...
        'SampleRate',musicInfo.SampleRate)
drumsMusic = filter(lpFilt,music);

% BMP Filter
bmpLowFreq = 2
bpmFilt = designfilt('lowpassiir','FilterOrder',1, ...
        'PassbandFrequency',bmpLowFreq,'PassbandRipple',0.2, ...
        'SampleRate',musicInfo.SampleRate)
compassMusic = filter(bpmFilt,music);
monoCompassMusic = (compassMusic(:,1) + compassMusic(:,2))/2;
bpmThreshold = 0.9*max(monoCompassMusic);
timeHitCompass = musicTime( monoCompassMusic > bpmThreshold );
timeHitCompass = timeHitCompass(2:end) - timeHitCompass(1:end-1);

% Mono Version - Reduce Pan effects
monoMusic = (music(:,1) + music(:,2))/2;
monoDrumsMusic = (drumsMusic(:,1) + drumsMusic(:,2))/2;



% Ploting
hold on
plot(monoMusic)
plot(drumsMusic)
hold off

% Spectogram
% spectrogram(music)
% Reduce Music rate for 7Hz
lowRateMusic = abs( downsample( music,6300 )*1024 );

% Prepare Music for Arduino
monoArLowQMusic = floor( (lowRateMusic(:,1) + lowRateMusic(:,2))/2 );
plot(monoArLowQMusic)
sizeArMusic = size(monoArLowQMusic)
csvwrite('music.csv',monoArLowQMusic');

% Create a header file with content
headerFile = fopen('music.h','w');
fprintf(headerFile,'#define SAMPLE_NUM %d\n',sizeArMusic(1));
fprintf(headerFile,'const int music[] = { ');
fclose(headerFile);

system('cat music.csv >> music.h; echo "};" >> music.h');
