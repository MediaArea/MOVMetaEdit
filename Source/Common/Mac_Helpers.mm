/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Common/Mac_Helpers.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
//---------------------------------------------------------------------------

Ztring makeUniqueFileName()
{
    NSString *unique = [[NSProcessInfo processInfo] globallyUniqueString];
    Ztring toReturn = Ztring().From_UTF8([unique UTF8String]);

    return toReturn;
}

Ztring makeTemporaryDirectoryForFile(const char *path)
{
    Ztring toReturn;
    NSURL *url = [NSURL fileURLWithPath:@(path)];

    if(!url)
        return toReturn;

    NSURL *temporaryDirectoryURL = [[NSFileManager defaultManager]
                                     URLForDirectory:NSItemReplacementDirectory
                                     inDomain:NSUserDomainMask
                                     appropriateForURL:url
                                     create:YES
                                     error:nil];

    if(!temporaryDirectoryURL)
        return toReturn;

    if(![temporaryDirectoryURL isFileURL])
        return toReturn;

    NSString *temporaryDirectoryPath = [temporaryDirectoryURL path];

    if(!temporaryDirectoryPath)
        return toReturn;

    toReturn = Ztring().From_UTF8([temporaryDirectoryPath UTF8String]);

    if(toReturn.size() && toReturn[toReturn.size()-1]!=__T('/'))
        toReturn+=__T("/");

    return toReturn;
}

bool deleteTemporaryDirectory(const char *path)
{
    bool toReturn = false;

    NSURL *url = [NSURL fileURLWithPath:@(path)];

    if(!url)
        return toReturn;

    toReturn = [[NSFileManager defaultManager] removeItemAtURL:url error:nil];

    return toReturn;
}
