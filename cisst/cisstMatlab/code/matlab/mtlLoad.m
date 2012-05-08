%
% $Id$
%
% Author(s):  Anton Deguet
% Created on: 2012-05-07
%
% (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
% Reserved.
%
% --- begin cisst license - do not edit ---
% 
% This software is provided "as is" under an open source license, with
% no warranty.  The complete license can be found in license.txt and
% http://www.cisst.org/cisst/license.txt.
%
% --- end cisst license ---

function [cisstMatlabFound, mtlLoadWarnings] = mtlLoad()
    cisst_root = getenv('CISST_ROOT');
    if strcmp(cisst_root, '')
        cisstMatlabFound = 0;
        mtlLoadWarnings = 'environment CISST_ROOT is not define';
        return;
    end
    addpath(strcat(cisst_root, '/include'));
    addpath(strcat(cisst_root, '/lib'));
    [cisstMatlabFound, mtlLoadWarnings] = loadlibrary('libcisstMatlab.dylib', 'cisstMatlab.h', ...
                                                      'includepath', strcat(cisst_root, '/include'), ...
                                                      'addheader', 'cisstMatlab/mtlCreateComponent.h', ...
                                                      'addheader', 'cisstMatlab/mtlCallFunction.h');
end
