//
//  GBOutputGenerator.h
//  appledoc
//
//  Created by Tomaz Kragelj on 28.11.10.
//  Copyright 2010 Gentle Bytes. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GBStoreProviding.h"

@class GBApplicationSettingsProvider;

/** The base class for all output generators.
 
 Output generator is an object that handles a specific spet while generating output. These are helper classes for `GBGenerator` class; each concrete subclass handles specifics for certain step. Generator just ties all of these together into properly ordered chain as required by command line parameters.
 */
@interface GBOutputGenerator : NSObject

///---------------------------------------------------------------------------------------
/// @name Initialization & disposal
///---------------------------------------------------------------------------------------

/** Returns autoreleased generator that work with the given `GBApplicationSettingsProvider` implementor.
 
 @param settingsProvider Application-wide settings provider to use for checking parameters.
 @return Returns initialized instance or `nil` if initialization fails.
 @exception NSException Thrown if the given application is `nil`.
 */
+ (id)generatorWithSettingsProvider:(id)settingsProvider;

/** Initializes the generator to work with the given `GBApplicationSettingsProvider` implementor.
 
 This is the designated initializer.
 
 @param settingsProvider Application-wide settings provider to use for checking parameters.
 @return Returns initialized instance or `nil` if initialization fails.
 @exception NSException Thrown if the given application is `nil`.
 */
- (id)initWithSettingsProvider:(id)settingsProvider;

///---------------------------------------------------------------------------------------
/// @name Generation handling
///---------------------------------------------------------------------------------------

/** Copies all files from the templates path to the output path as defined in assigned `settings`, replicating the directory structure and stores all detected template files to `templateFiles` dictionary.
 
 The method uses `[GBApplicationSettingsProvider templatesPath]` as the base path for templates and `[GBApplicationSettingsProvider outputPath]` as the base path for output. In both cases, `outputSubpath` is used to determine the source and destination subdirectories. It then copies all files from template path to the output path, including the whole directory structure. If any special template file is found at source path, it is not copied! Template files are identified by having a `-template` suffix followed by optional extension. For example `object-template.html`. As this message prepares the ground for actual generation, it should be sent before any other messages (i.e. before `generateOutput:`).
 
 To further aid subclasses, the method reads out all template files in templates path and stores them to `templateFiles` dictionary. Each template file is stored with a key correspoding to it's filename, including the subdirectory within the base template path and extension.
  
 @warning *Note:* This message is intended to be sent from higher-level generator objects. Although it would present no error to run it several times, in most circumstances subclasses don't need to send it manually. If copying fails, a warning is logged and copying is stopped. Depending of type of failure, the method either returns `YES` or `NO`. If copying of all files is succesful, but reading or clearing template or ignored files fails, the operation is still considered succesful, so `YES` is returned. However if replicating the directory structure or copying files fails, this is considered an error and `NO` is returned. In such case, clients should abort further processing.


 @param error If copying fails, error description is returned here.
 @return Returns `YES` if all files were succesfully copied, `NO` otherwise.
 @see generateOutputWithStore:error:
 */
- (BOOL)copyTemplateFilesToOutputPath:(NSError **)error;

/** Generates the output at the proper subdirectory of the output path as defined in assigned `settings`.
 
 This is the most important method of the `GBOutputGenerator` class. It generates all required output. It is intended to be overriden in subclasses. Default implementation assigns the given store and returns YES. Subclasses must call super class implementation before anything else!
 
 @param store The `GBStoreProviding` object that holds the store with all parsed and processed data.
 @param error If generation fails, error description is returned here.
 @see copyTemplateFilesToOutputPath:
 @see writeString:toFile:error:
 @see store
 */
- (BOOL)generateOutputWithStore:(id<GBStoreProviding>)store error:(NSError **)error;

/** Writes the given string to the given path, creating all necessary directories if they don't exist.
 
 This method is intended to be used from subclass, in most cases from `generateOutputWithStore:error:`.
 
 @param string The string to write.
 @param path The path and filename to write to.
 @param error If writting fails, error description is returned here.
 @return Returns `YES` is writting succeds, `NO` otherwise.
 @see generateOutputWithStore:error:
 */
- (BOOL)writeString:(NSString *)string toFile:(NSString *)path error:(NSError **)error;

///---------------------------------------------------------------------------------------
/// @name Subclass parameters and helpers
///---------------------------------------------------------------------------------------

/** Searches `templateFiles` dictionary for a key ending with the given suffix and returns the whole key if found.
 
 If the key is not found, `nil` is returned. This method is useful for finding keys for which we only know partial name (ussually the name of the file, but not the path to it).
 
 @param suffix Template file suffix to search for.
 @return Returns full key if found, `nil` otherwise.
 @see templateFiles
 */
- (NSString *)templateFileKeyEndingWith:(NSString *)suffix;

/** The dictionary of all template files detected within `copyTemplateFilesToOutputPath:`.
 
 Each object has a key of template file name and relative path from `templateUserPath`. The keys are mapped to `GBTemplateHandler` instances associated with the template.
 
 This is intended to be used within subclasses only. Dictionary contents are automatically updated and should not be changed by subclasses.
 
 @see copyTemplateFilesToOutputPath:
 @see templateFileKeyEndingWith:
 */
@property (readonly) NSMutableDictionary *templateFiles;

/** Returns user-friendly template path string including `outputSubpath`. 
 
 This uses the same string as entered by the user when starting the application. Send `stringByStandardizingPath` message to the returned value before using it!
 
 @see outputUserPath
 */
@property (readonly) NSString *templateUserPath;

/** Returns the output path including `outputSubpath`. 
 
 This uses the same string as entered by the user when starting the application. Send `stringByStandardizingPath` message to the returned value before using it!
 
 @see templateUserPath
 */
@property (readonly) NSString *outputUserPath;

/** Returns the `GBOutputGenerator` that was used just before this one or `nil` if this is the first generator in this session.
 
 This is useful for subclasses that rely on files generated by previous generator. For example, we can get the location of generated files so we can further manipulate them. If a concrete subclass requires previous generator, and none is provided, it should throw an exception from `generateOutputWithStore:error:`!
 */
@property (retain) GBOutputGenerator *previousGenerator;

/** The store as assigned to `generateOutput`.
 
 @see generateOutputWithStore:error:
 */
@property (readonly, retain) id<GBStoreProviding> store;

///---------------------------------------------------------------------------------------
/// @name Generation parameters
///---------------------------------------------------------------------------------------

/** Returns the path relative to main output path, where all generated data is stored.
 
 At the same this, this also defines the path relative to main templates path, where all template files for this output generator are stored. Default implementation simply returns empty string, each subclass is supposed to override and return prover value.
 
 @see copyTemplateFilesToOutputPath:
  */
@property (readonly) NSString *outputSubpath;

/** The `GBApplicationSettingsProvider` object that provides application-wide settings for this session. 
 */
@property (retain) GBApplicationSettingsProvider *settings;

@end
