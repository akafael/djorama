%% Music Processing script
% 
% @author Rafael Lima

function processMusic()
    
   % Load Wav Music
   musicFileName = 'Daft Punk - One More Time.wav'
   musicInfo = audioinfo(musicFileName)
   music = audioread(musicFileName);

   % play music
   sound(music)
   % stop music
   clear sound

   % spectogram
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
end