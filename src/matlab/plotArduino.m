%% Arduino MIC Test
% Use with testMicSensor.ino
function x = plotArduino(port)

    delete(instrfind({'Port'},{port}));
    serialPort = serial(port);
    serialPort.BaudRate = 115200; % High BaudRate cause buffer skipping
    fopen(serialPort);

    buffersize = 3000;
    x = zeros(buffersize,3);
    i = 1;

    axisPlot = axes()

    % Load Wav Music
    musicFileName = 'Daft Punk - The Brainwasher.wav'
    %musicFileName = 'Daft Punk - One More Time.wav'
    musicInfo = audioinfo(musicFileName)
    music = audioread(musicFileName);
    musicTime = (1:musicInfo.TotalSamples)/musicInfo.SampleRate;

    % play music
    playerMusic = audioplayer(music,musicInfo.SampleRate,musicInfo.BitsPerSample)
    playerMusic.play()
    tic

    while ((~isempty(serialPort)) && strcmp(serialPort.Status,'open'))
        % Read Data
        x(i,3) = toc;
        str = fgetl(serialPort);
        %str = strtrim(str);
        str = strsplit(str, ' ')

        if(size(str,2) == 2)
            x(i,1) = str2double(str{2});
            x(i,2) = str2double(str{1});
        end

        % Update Index
        if (i >= buffersize)
            break;
        else
            i = i+1;
        end
    end

    % Remove Outliers
    y = x(:,1)
    y = y.*(y<600).*(y>400)
    y(y<400) = mean(y)

    % Normalise
    range = max(y) - mean(y)
    y = (y - mean(y))/range;

    % Ajust output
    x(:,1) = y
    x = x( x(:,2) > 0,:) % Remove Zeros
    x(:,2) = x(:,2)/1000 % Ajust time unit for seconds
    x(:,2) = x(:,2) + x(1,3) - (x(3,3) - x(2,3)) % Add bias capture start
    timeCapture = max(x(:,3))

    % Plot
    hold on
    plot(musicTime(musicTime < timeCapture),music(musicTime < timeCapture,1))
    plot(x(:,2) + 0.45,x(:,1))
    hold off

    % stop music
    playerMusic.stop()
end
