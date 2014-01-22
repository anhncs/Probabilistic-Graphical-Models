function submit(partId, webSubmit)
%SUBMIT Submit your code and output to the pgm-class servers
%   SUBMIT() will connect to the pgm-class server and submit your solution
%   There is no penalty for submitting, so go ahead and try this!
%
%   If this function does not work for you, use the web-submission mechanism.
%   Call the submitWeb function, which will produce a file for the part you
%   wish to submit. Then, submit the file to the class servers using the 
%   "Web Submission" button on the Programming Assignments page on the course
%   website.
%   
%   webSubmit is a boolean variable that specifies whether to prepare
%   a file for web-submission (if webSubmit = 1) or to submit directly
%   to the server (if webSubmit = 0, or is not specified); you should call
%   submitWeb if you want to do a web-submission.
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

fprintf('======================================================\n');
fprintf('                 Honor Code Statement:\n');
fprintf('------------------------------------------------------\n');
fprintf('The Honor Code is an undertaking of the students,\n');
fprintf('individually and collectively, that they will not\n');
fprintf('give or receive unpermitted aid in class work and\n');
fprintf('will do their share and take an active part in seeing\n');
fprintf('to it that others as well as themselves uphold the\n');
fprintf('spirit and letter of the Honor Code.\n\n');
fprintf('Sharing, collaboration, or looking at any code related\n');
fprintf('to Programming Assignments that is not your own is\n');
fprintf('considered a violation of the Honor Code.\n');
fprintf('------------------------------------------------------\n');
fprintf('By submitting the Programming Assignment, I\n');
fprintf('acknowledge and accept the Honor Code.\n');
fprintf('======================================================\n\n');

  fprintf('==\n== [pgm-class] Submitting Solutions | Programming Assignment %s\n==\n', ...
	  homework_id());
  
  if ~exist('partId', 'var') || isempty(partId)
    partId = promptPart();
  end
  
  if ~exist('webSubmit', 'var') || isempty(webSubmit)
    webSubmit = 0; % submit directly by default 
  end

  % Check valid partId
  partNames = validParts();
  if ~isValidPartId(partId)
    fprintf('!! Invalid assignment part selected.\n');
    fprintf('!! Expected an integer from 1 to %d.\n', numel(partNames) + 1);
    fprintf('!! Submission Cancelled\n');
    return
  end

  if ~exist('pgm_login_data.mat','file')
    [login password] = loginPrompt();
    save('pgm_login_data.mat','login','password');
  else  
    load('pgm_login_data.mat');
    [login password] = quickLogin(login,password);
    save('pgm_login_data.mat','login','password');
  end
  
  if isempty(login)
    fprintf('!! Submission Cancelled\n');
    return
  end

  fprintf('\n== Connecting to pgm-class ... '); 
  if exist('OCTAVE_VERSION') 
    fflush(stdout);
  end


  % Setup submit list
  if partId == numel(partNames) + 1
    submitParts = 1:numel(partNames);
  else
    submitParts = [partId];
  end

  for s = 1:numel(submitParts)
    % Submit this part
    partId = submitParts(s);

    for thisPartId = subParts(partId)
      % hack for the last part
      if (thisPartId==8)
          continue;
      end
      if (~webSubmit) % submit directly to server
  
        [login, ch, signature, auxstring] = getChallenge(login, thisPartId);
        if isempty(login) || isempty(ch) || isempty(signature)
          % Some error occured, error string in first return element.
          fprintf('\n!! Error: %s\n\n', login);
          return
        end

        % Attempt Submission with Challenge
        ch_resp = challengeResponse(login, password, ch);

        [result, str] = submitSolution(login, ch_resp, thisPartId, ...
               output(thisPartId, auxstring), source(partId), signature);

        if (~isTest(thisPartId))
          partName = partNames{partId};
        else
          partName = [partNames{partId} ' (test)'];
        end

        fprintf('\n== [pgm-class] Submitted Assignment %s - Part %d - %s\n', ...
          homework_id(), partId, partName);
        fprintf('== %s\n', strtrim(str));

        if exist('OCTAVE_VERSION')
          fflush(stdout);
        end
        
      else % make web submission files
        
        [result] = submitSolutionWeb(login, thisPartId, output(thisPartId), ...
                            source(partId));
        result = base64encode(result);

        if (~isTest(thisPartId))
          partType = 'sample';
        else
          partType = 'test';
        end
                
        fprintf('\nSave as submission file [submit_pa%s_part%d_%s.txt (enter to accept default)]:', ...
          homework_id(), partId, partType);
        saveAsFile = input('', 's');
        if (isempty(saveAsFile))
          saveAsFile = sprintf('submit_pa%s_part%d_%s.txt', homework_id(), partId, partType);
        end

        fid = fopen(saveAsFile, 'w');
        if (fid)
          fwrite(fid, result);
          fclose(fid);
          fprintf('\nSaved your solutions to %s.\n\n', saveAsFile);
          fprintf(['You can now submit your solutions using the ' ...
             'Web Submission button\non the Assignments page\n']);
        else
          fprintf('Unable to save to %s\n\n', saveAsFile);
          fprintf(['You can create a submission file by saving the \n' ...
             'following text in a file: (press enter to continue)\n\n']);
          pause;
          fprintf(result);
        end                                  
        
      end      
    end
  end
end


% ====================== SERVER CONFIGURATION ===========================

% ***************** REMOVE -staging WHEN YOU DEPLOY *********************
function url = site_url()
  url = 'http://www.coursera.org/pgm';
end

function url = challenge_url()

  url = [site_url() '/assignment/challenge'];
end

function url = submit_url()
  url = [site_url() '/assignment/submit'];
end

% ================== CONFIGURABLES FOR EACH HOMEWORK ==================

function id = homework_id() 
  id = '9';
end

function [partNames] = validParts()
  partNames = { 'EM_cluster', ...
		'EM_HMM', ...
        'RecognizeActions', ...
		'RecognizeUnknownActions', ...
	      };
end

function srcs = sources()
% Separated by part
  srcs = { { 'EM_cluster.m' }, ... %1
	   { 'EM_HMM.m' }, ... %3    
	   { 'RecognizeActions.m' }, ... %5
	   { 'RecognizeUnknownActions.m', 'YourMethod.txt' } %7
	 };
end

% defines the shown part to back-end part mappings 
function parts = subParts(partId)
  first = 2 * (partId - 1) + 1;
  parts = [first, first + 1];
end

% specifies which parts are test parts
function result = isTest(partId)
  if (mod(partId, 2) == 0)
      result = true;
  else
      result = false;
  end
end

function out = output(partId, auxstring)

  if partId == 1 %EM_cluster

    load 'PA9SampleCases.mat';
    [P ll CP] = EM_cluster(exampleINPUT.t1a1, exampleINPUT.t1a2, exampleINPUT.t1a3, exampleINPUT.t1a4);
    tmp = [[P.c] [P.clg.sigma_x] [P.clg.sigma_y] [P.clg.sigma_angle] [ll'] [CP(:)']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end
    
  elseif partId == 2 %EM_cluster

    load 'submit_input.mat';
    [P ll CP] = EM_cluster(INPUT.t1a1, INPUT.t1a2, INPUT.t1a3, INPUT.t1a4);
    tmp = [[P.c] [P.clg.sigma_x] [P.clg.sigma_y] [P.clg.sigma_angle] [ll'] [CP(:)']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end

  elseif partId == 3 %EM_HMM

    load 'PA9SampleCases.mat';
    [P ll CP PP] = EM_HMM(exampleINPUT.t2a1, exampleINPUT.t2a2, exampleINPUT.t2a3, exampleINPUT.t2a4, exampleINPUT.t2a5, exampleINPUT.t2a6);
    tmp = [[P.c] [P.clg.sigma_x] [P.clg.sigma_y] [P.clg.sigma_angle] [ll'] [CP(:)'] [PP(:)']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end

  elseif partId == 4 %EM_HMM

    load 'submit_input.mat';
    [P ll CP PP] = EM_HMM(INPUT.t2a1, INPUT.t2a2, INPUT.t2a3, INPUT.t2a4, INPUT.t2a5, INPUT.t2a6);
    tmp = [[P.c] [P.clg.sigma_x] [P.clg.sigma_y] [P.clg.sigma_angle] [ll'] [CP(:)'] [PP(:)']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end

  elseif partId == 5 %RecognizeActions

    load 'PA9SampleCases.mat';
    [acc pl] = RecognizeActions(exampleINPUT.t3a1, exampleINPUT.t3a2, exampleINPUT.t3a3, exampleINPUT.t3a4);
    tmp = [[acc] [pl']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end
    
  elseif partId == 6 %RecognizeActions

    load 'submit_input.mat';
    [acc pl] = RecognizeActions(INPUT.t3a1, INPUT.t3a2, INPUT.t3a3, INPUT.t3a4);
    tmp = [[acc] [pl']];
    out = '';
    for idx = 1:length(tmp)
        out = [out ' ' SerializeFloat(tmp(idx))];
    end

  elseif partId == 7 %RecognizeUnknownActions

	if (~exist('Predictions.mat'))
		warning('You have not run SavePrediction.m before submitting.');
		out = '';
		return;
	end
	load Predictions.mat;
	v = yourPredictions(:);
	v = [numel(v); v];
	out = SerializeIntVector(v);

  end
  
  % end of output function.
  
end


function out = SerializeFloat( x )
  out = sprintf('%.4f', x);
end

function out = SerializeFactorsFg(F)
% Serializes a factor struct array into the .fg format for libDAI
% http://cs.ru.nl/~jorism/libDAI/doc/fileformats.html
%
% To avoid incompatibilities with EOL markers, make sure you write the
% string to a file using the appropriate file type ('wt' for windows, 'w'
% for unix)

  lines = cell(5*numel(F) + 1, 1);

  lines{1} = sprintf('%d\n', numel(F));
  lineIdx = 2;
  for i = 1:numel(F)
    lines{lineIdx} = sprintf('\n%d\n', numel(F(i).var));
    lineIdx = lineIdx + 1;

    lines{lineIdx} = sprintf('%s\n', num2str(F(i).var(:)')); % ensure that we put in a row vector
    lineIdx = lineIdx + 1;
    
    lines{lineIdx} = sprintf('%s\n', num2str(F(i).card(:)')); % ensure that we put in a row vector
    lineIdx = lineIdx + 1;

    lines{lineIdx} = sprintf('%d\n', numel(F(i).val));
    lineIdx = lineIdx + 1;

    % Internal storage of factor vals is already in the same indexing order
    % as what libDAI expects, so we don't need to convert the indices.
    vals = [0:(numel(F(i).val) - 1); F(i).val(:)'];
    lines{lineIdx} = sprintf('%d %0.8g\n', vals);
    lineIdx = lineIdx + 1;
  end

  out = sprintf('%s', lines{:});

end


function out = SerializeMEUOptimization(meu, optdr)
  optdr = SortFactorVars(optdr);
  optdr_part = SerializeFactorsFg(optdr);
  out = sprintf('%s\n%.4f\n', optdr_part, meu);
end


function f = SortAllFactors(factors)

  for i = 1:length(factors)
    factors(i) = SortFactorVars(factors(i));
  end

  varMat = vertcat(factors(:).var);
  [unused, order] = sortrows(varMat);

  f = factors(order);

end

function G = SortFactorVars(F)

  [sortedVars, order] = sort(F.var);
  G.var = sortedVars;

  G.card = F.card(order);
  G.val = zeros(numel(F.val), 1);

  assignmentsInF = IndexToAssignment(1:numel(F.val), F.card);
  assignmentsInG = assignmentsInF(:,order);
  G.val(AssignmentToIndex(assignmentsInG, G.card)) = F.val;

end

% ========================= CHALLENGE HELPERS =========================

function src = source(partId)
  src = '';
  src_files = sources();
  if partId <= numel(src_files)
    flist = src_files{partId};
    for i = 1:numel(flist)
      fid = fopen(flist{i});
      if (fid == -1) 
	error('Error opening %s (is it missing?)', flist{i});
      end
      line = fgets(fid);
      while ischar(line)
	src = [src line];            
	line = fgets(fid);
      end
      fclose(fid);
      src = [src '||||||||'];
    end
  end
end

function ret = isValidPartId(partId)
  partNames = validParts();
  ret = (~isempty(partId)) && (partId >= 1) && (partId <= numel(partNames) + 1);
end

function partId = promptPart()
  fprintf('== Select which part(s) to submit:\n', ...
	  homework_id());
  partNames = validParts();
  srcFiles = sources();
  for i = 1:numel(partNames)
    fprintf('==   %d) %s [', i, partNames{i});
    fprintf(' %s ', srcFiles{i}{:});
    fprintf(']\n');
  end
  fprintf('==   %d) All of the above \n==\nEnter your choice [1-%d]: ', ...
	  numel(partNames) + 1, numel(partNames) + 1);
  selPart = input('', 's');
  partId = str2num(selPart);
  if ~isValidPartId(partId)
    partId = -1;
  end
end

function [email,ch,signature,auxstring] = getChallenge(email, part)
  str = urlread(challenge_url(), 'post', {'email_address', email, 'assignment_part_sid', [homework_id() '-' num2str(part)], 'response_encoding', 'delim'});

  str = strtrim(str);
  r = struct;
  while(numel(str) > 0)
    [f, str] = strtok (str, '|');
    [v, str] = strtok (str, '|');
    r = setfield(r, f, v);
  end

  email = getfield(r, 'email_address');
  ch = getfield(r, 'challenge_key');
  signature = getfield(r, 'state');
  auxstring = getfield(r, 'challenge_aux_data');
end


function [result, str] = submitSolution(email, ch_resp, part, output, ...
                                        source, signature)

  params = {'assignment_part_sid', [homework_id() '-' num2str(part)], ...
	    'email_address', email, ...
	    'submission', base64encode(output, ''), ...
	    'submission_aux', base64encode(source, ''), ...
	    'challenge_response', ch_resp, ...
	    'state', signature};

  str = urlread(submit_url(), 'post', params);

  % Parse str to read for success / failure
  result = 0;

end

% =========================== LOGIN HELPERS ===========================

function [login password] = loginPrompt()
% Prompt for password
  [login password] = basicPrompt();

  if isempty(login) || isempty(password)
    login = []; password = [];
  end
end


function [login password] = basicPrompt()
  login = input('Login (Email address): ', 's');
  password = input('Submission Password (from Assignments page): ', 's');
end

function [login password] = quickLogin(login,password)
  cont_token = input(['You are currently logged in as ' login '. Is this you? (y/n - type n to reenter password)'],'s');
  if(isempty(cont_token) || cont_token(1)=='Y'||cont_token(1)=='y')
    return;
  else
    [login password] = loginPrompt();
  end
end

function [str] = challengeResponse(email, passwd, challenge)
  str = sha1([challenge passwd]);
end


% =============================== SHA-1 ================================

function hash = sha1(str)

% Initialize variables
  h0 = uint32(1732584193);
  h1 = uint32(4023233417);
  h2 = uint32(2562383102);
  h3 = uint32(271733878);
  h4 = uint32(3285377520);

  % Convert to word array
  strlen = numel(str);

  % Break string into chars and append the bit 1 to the message
  mC = [double(str) 128];
  mC = [mC zeros(1, 4-mod(numel(mC), 4), 'uint8')];

  numB = strlen * 8;
  if exist('idivide')
    numC = idivide(uint32(numB + 65), 512, 'ceil');
  else
    numC = ceil(double(numB + 65)/512);
  end
  numW = numC * 16;
  mW = zeros(numW, 1, 'uint32');

  idx = 1;
  for i = 1:4:strlen + 1
    mW(idx) = bitor(bitor(bitor( ...
	bitshift(uint32(mC(i)), 24), ...
	bitshift(uint32(mC(i+1)), 16)), ...
			  bitshift(uint32(mC(i+2)), 8)), ...
		    uint32(mC(i+3)));
    idx = idx + 1;
  end

  % Append length of message
  mW(numW - 1) = uint32(bitshift(uint64(numB), -32));
  mW(numW) = uint32(bitshift(bitshift(uint64(numB), 32), -32));

  % Process the message in successive 512-bit chs
  for cId = 1 : double(numC)
    cSt = (cId - 1) * 16 + 1;
    cEnd = cId * 16;
    ch = mW(cSt : cEnd);

    % Extend the sixteen 32-bit words into eighty 32-bit words
    for j = 17 : 80
      ch(j) = ch(j - 3);
      ch(j) = bitxor(ch(j), ch(j - 8));
      ch(j) = bitxor(ch(j), ch(j - 14));
      ch(j) = bitxor(ch(j), ch(j - 16));
      ch(j) = bitrotate(ch(j), 1);
    end

    % Initialize hash value for this ch
    a = h0;
    b = h1;
    c = h2;
    d = h3;
    e = h4;

    % Main loop
    for i = 1 : 80
      if(i >= 1 && i <= 20)
	f = bitor(bitand(b, c), bitand(bitcmp(b), d));
	k = uint32(1518500249);
      elseif(i >= 21 && i <= 40)
	f = bitxor(bitxor(b, c), d);
	k = uint32(1859775393);
      elseif(i >= 41 && i <= 60)
	f = bitor(bitor(bitand(b, c), bitand(b, d)), bitand(c, d));
	k = uint32(2400959708);
      elseif(i >= 61 && i <= 80)
	f = bitxor(bitxor(b, c), d);
	k = uint32(3395469782);
      end

      t = bitrotate(a, 5);
      t = bitadd(t, f);
      t = bitadd(t, e);
      t = bitadd(t, k);
      t = bitadd(t, ch(i));
      e = d;
      d = c;
      c = bitrotate(b, 30);
      b = a;
      a = t;

    end
    h0 = bitadd(h0, a);
    h1 = bitadd(h1, b);
    h2 = bitadd(h2, c);
    h3 = bitadd(h3, d);
    h4 = bitadd(h4, e);

  end

  hash = reshape(dec2hex(double([h0 h1 h2 h3 h4]), 8)', [1 40]);
  
  hash = lower(hash);
end
  
function ret = bitadd(iA, iB)
  ret = double(iA) + double(iB);
  ret = bitset(ret, 33, 0);
  ret = uint32(ret);
end
               
function ret = bitrotate(iA, places)
  t = bitshift(iA, places - 32);
  ret = bitshift(iA, places);
  ret = bitor(ret, t);
               end
               
% =========================== Base64 Encoder ============================
% Thanks to Peter John Acklam
%
               
function y = base64encode(x, eol)
%BASE64ENCODE Perform base64 encoding on a string.
%
%   BASE64ENCODE(STR, EOL) encode the given string STR.  EOL is the line ending
%   sequence to use; it is optional and defaults to '\n' (ASCII decimal 10).
%   The returned encoded string is broken into lines of no more than 76
%   characters each, and each line will end with EOL unless it is empty.  Let
%   EOL be empty if you do not want the encoded string broken into lines.
%
%   STR and EOL don't have to be strings (i.e., char arrays).  The only
%   requirement is that they are vectors containing values in the range 0-255.
%
%   This function may be used to encode strings into the Base64 encoding
%   specified in RFC 2045 - MIME (Multipurpose Internet Mail Extensions).  The
%   Base64 encoding is designed to represent arbitrary sequences of octets in a
%   form that need not be humanly readable.  A 65-character subset
%   ([A-Za-z0-9+/=]) of US-ASCII is used, enabling 6 bits to be represented per
%   printable character.
%
%   Examples
%   --------
%
%   If you want to encode a large file, you should encode it in chunks that are
%   a multiple of 57 bytes.  This ensures that the base64 lines line up and
%   that you do not end up with padding in the middle.  57 bytes of data fills
%   one complete base64 line (76 == 57*4/3):
%
%   If ifid and ofid are two file identifiers opened for reading and writing,
%   respectively, then you can base64 encode the data with
%
%      while ~feof(ifid)
%         fwrite(ofid, base64encode(fread(ifid, 60*57)));
%      end
%
%   or, if you have enough memory,
%
%      fwrite(ofid, base64encode(fread(ifid)));
%
%   See also BASE64DECODE.
  
%   Author:      Peter John Acklam
%   Time-stamp:  2004-02-03 21:36:56 +0100
%   E-mail:      pjacklam@online.no
%   URL:         http://home.online.no/~pjacklam
  
  if isnumeric(x)
    x = num2str(x);
  end
  
  % make sure we have the EOL value
  if nargin < 2
    eol = sprintf('\n');
  else
    if sum(size(eol) > 1) > 1
      error('EOL must be a vector.');
    end
    if any(eol(:) > 255)
      error('EOL can not contain values larger than 255.');
    end
  end
  
  if sum(size(x) > 1) > 1
    error('STR must be a vector.');
  end
  
  x   = uint8(x);
  eol = uint8(eol);
  
  ndbytes = length(x);                 % number of decoded bytes
  nchunks = ceil(ndbytes / 3);         % number of chunks/groups
  nebytes = 4 * nchunks;               % number of encoded bytes
  
  % add padding if necessary, to make the length of x a multiple of 3
  if rem(ndbytes, 3)
    x(end+1 : 3*nchunks) = 0;
  end
               
  x = reshape(x, [3, nchunks]);        % reshape the data
  y = repmat(uint8(0), 4, nchunks);    % for the encoded data
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Split up every 3 bytes into 4 pieces
  %
  %    aaaaaabb bbbbcccc ccdddddd
  %
  % to form
  %
  %    00aaaaaa 00bbbbbb 00cccccc 00dddddd
  %
  y(1,:) = bitshift(x(1,:), -2);                  % 6 highest bits of x(1,:)
  
  y(2,:) = bitshift(bitand(x(1,:), 3), 4);        % 2 lowest bits of x(1,:)
  y(2,:) = bitor(y(2,:), bitshift(x(2,:), -4));   % 4 highest bits of x(2,:)
  
  y(3,:) = bitshift(bitand(x(2,:), 15), 2);       % 4 lowest bits of x(2,:)
  y(3,:) = bitor(y(3,:), bitshift(x(3,:), -6));   % 2 highest bits of x(3,:)
  
  y(4,:) = bitand(x(3,:), 63);                    % 6 lowest bits of x(3,:)
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Now perform the following mapping
  %
  %   0  - 25  ->  A-Z
  %   26 - 51  ->  a-z
  %   52 - 61  ->  0-9
  %   62       ->  +
  %   63       ->  /
  %
  % We could use a mapping vector like
  %
  %   ['A':'Z', 'a':'z', '0':'9', '+/']
  %
  % but that would require an index vector of class double.
  %
  z = repmat(uint8(0), size(y));
  i =           y <= 25;  z(i) = 'A'      + double(y(i));
  i = 26 <= y & y <= 51;  z(i) = 'a' - 26 + double(y(i));
  i = 52 <= y & y <= 61;  z(i) = '0' - 52 + double(y(i));
  i =           y == 62;  z(i) = '+';
  i =           y == 63;  z(i) = '/';
  y = z;
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Add padding if necessary.
  %
  npbytes = 3 * nchunks - ndbytes;     % number of padding bytes
  if npbytes
    y(end-npbytes+1 : end) = '=';     % '=' is used for padding
  end
  
  if isempty(eol)
    
    % reshape to a row vector
    y = reshape(y, [1, nebytes]);
    
  else
    
    nlines = ceil(nebytes / 76);      % number of lines
    neolbytes = length(eol);          % number of bytes in eol string
    
    % pad data so it becomes a multiple of 76 elements
    y = [y(:) ; zeros(76 * nlines - numel(y), 1)];
    y(nebytes + 1 : 76 * nlines) = 0;
    y = reshape(y, 76, nlines);
    
    % insert eol strings
    eol = eol(:);
    y(end + 1 : end + neolbytes, :) = eol(:, ones(1, nlines));
    
    % remove padding, but keep the last eol string
    m = nebytes + neolbytes * (nlines - 1);
    n = (76+neolbytes)*nlines - neolbytes;
    y(m+1 : n) = '';
    
    % extract and reshape to row vector
    y = reshape(y, 1, m+neolbytes);
    
  end
  
  % output is a character array
  y = char(y);
  
end

function out = SerializeIntVector(x)
  % Serializes an integer vector. 
  numLines = length(x);
  lines = cell(numLines,1);
  for i=1:numLines
    lines{i} = sprintf('%d\n', x(i));
  end
  out = sprintf('%s', lines{:});

end
