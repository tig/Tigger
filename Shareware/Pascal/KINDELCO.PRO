  Procedure KindelCo (Year : integer);
    begin
      Center (8, '  ��    ��  ����  ���     ��  �������   �������  ��       ������    ������   ');
      Center (9, '  ��   ��    ��   ����    ��  ��    ��  ��       ��      ��    ��  ��    ��  ');
      Center (10,'  ��  ��     ��   �� ��   ��  ��    ��  ��       ��      ��        ��    ��  ');
      Center (11,'  �����      ��   ��  ��  ��  ��    ��  �����    ��      ��        ��    ��  ');
      Center (12,'  ��  ��     ��   ��   �� ��  ��    ��  ��       ��      ��        ��    ��  ');
      Center (13,'  ��   ��    ��   ��    ����  ��    ��  ��       ��      ��    ��  ��    ��  ');
      Center (14,'  ��    ��  ����  ��     ���  �������   �������  �������  ������    ������   ');
      GotoXY (22, 15); Write ('Software Systems, copyright (c) ', Year);
      Frame (18, 17, 61, 24);
      LowVideo;
{! 1. ^The effects of HighVideo,LowVideo,NormVideo are different in Turbo 4.0.}
      Center (18, 'If you find this program useful');
      Center (19, 'please send what you think it`s worth to:');
      highvideo;
{! 2. ^The effects of HighVideo,LowVideo,NormVideo are different in Turbo 4.0.}
      Center (20, 'Charles E. Kindel');
      Center (21, 'KindelCo Software Systems');
      Center (22, '1632 E. Linden');
      Center (23, 'Tucson, AZ  85719');
    end;
