-- Mirix Operating System Make
-- BSD Make implementation for Classic Mac OS (System 7, 8, 9)
-- Pure Classic AppleScript - Compatible with Mac OS 8.6
--
-- Usage: Run this script and it will look for a "Makefile" in the same folder

property appName : "Mirix Operating System Make"
property appVersion : "1.0 Classic"

-- Initialize globals
on run
	-- Welcome message
	display dialog "Welcome to " & appName & " v" & appVersion & return & return & Â
		"This is a BSD Make implementation for Classic Mac OS." & return & return & Â
		"Looking for Makefile in current folder..." buttons {"OK"} default button 1
	
	try
		set makefilePath to findMakefile()
		set makefileData to parseMakefile(makefilePath)
		
		-- Ask user which target to build
		set targetChoice to chooseTarget(makefileData)
		
		if targetChoice is not "" then
			-- Build the target
			set buildResult to buildTarget(targetChoice, makefileData)
			
			if buildResult is true then
				display dialog "Build completed successfully!" & return & return & Â
					"Target: " & targetChoice buttons {"OK"} default button 1
			else
				display dialog "Build failed!" & return & return & Â
					"Target: " & targetChoice buttons {"OK"} default button 1
			end if
		end if
		
	on error errMsg number errNum
		display dialog "Error: " & errMsg & return & "Error number: " & errNum buttons {"OK"} default button 1
	end try
end run

-- Find the Makefile
on findMakefile()
	tell application "Finder"
		set scriptFolder to container of (path to me)
		
		-- Try "Makefile" first
		try
			set makefileRef to file "Makefile" of scriptFolder
			return makefileRef as alias
		end try
		
		-- Try "makefile"
		try
			set makefileRef to file "makefile" of scriptFolder
			return makefileRef as alias
		end try
		
		-- No makefile found
		error "No Makefile found in current folder" number 1000
	end tell
end findMakefile

-- Parse the Makefile (simplified for Classic Mac OS)
on parseMakefile(makefilePath)
	set makefileText to read makefilePath
	set lineList to paragraphs of makefileText
	
	set targetsList to {}
	set depsList to {}
	set recipesList to {}
	set variablesList to {}
	set phonyList to {}
	set defaultTarget to ""
	set currentTarget to ""
	
	-- Set default variables for Classic Mac OS
	set end of variablesList to {varName:"CC", varValue:"MrC"}
	set end of variablesList to {varName:"LD", varValue:"PPCLink"}
	set end of variablesList to {varName:"CFLAGS", varValue:""}
	
	repeat with i from 1 to count of lineList
		set theLine to item i of lineList
		
		-- Skip empty lines and comments
		if theLine is "" then
			-- skip
		else if theLine starts with "#" then
			-- skip comment
		else if theLine starts with "	" then
			-- Recipe line
			if currentTarget is not "" then
				set recipeCmd to trimLeft(theLine)
				set end of recipesList to {tgtName:currentTarget, cmd:recipeCmd}
			end if
		else
			-- Check for .PHONY
			if theLine starts with ".PHONY:" then
				set phonyLine to text 8 thru -1 of theLine
				set phonyTargets to splitString(phonyLine, " ")
				repeat with pt in phonyTargets
					set ptText to pt as text
					set ptText to trim(ptText)
					if ptText is not "" then
						set end of phonyList to ptText
					end if
				end repeat
				
				-- Check for variable
			else if theLine contains "=" then
				set eqPos to offset of "=" in theLine
				if eqPos > 0 then
					set varName to trim(text 1 thru (eqPos - 1) of theLine)
					set varValue to trim(text (eqPos + 1) thru -1 of theLine)
					
					-- Handle := and ?=
					if varName ends with ":" then
						set varName to text 1 thru -2 of varName
						set varName to trim(varName)
					else if varName ends with "?" then
						set varName to text 1 thru -2 of varName
						set varName to trim(varName)
					end if
					
					-- Expand variables in value
					set varValue to expandVars(varValue, variablesList)
					
					-- Store variable
					set foundVar to false
					repeat with j from 1 to count of variablesList
						set vInfo to item j of variablesList
						if varName of vInfo is varName then
							set varValue of vInfo to varValue
							set foundVar to true
							exit repeat
						end if
					end repeat
					if not foundVar then
						set end of variablesList to {varName:varName, varValue:varValue}
					end if
				end if
				
				-- Check for target line
			else if theLine contains ":" then
				set colonPos to offset of ":" in theLine
				set targetPart to trim(text 1 thru (colonPos - 1) of theLine)
				set depsPart to trim(text (colonPos + 1) thru -1 of theLine)
				
				-- Expand variables
				set targetPart to expandVars(targetPart, variablesList)
				set depsPart to expandVars(depsPart, variablesList)
				
				-- Handle target
				set tgtList to splitString(targetPart, " ")
				repeat with tgt in tgtList
					set tgtText to tgt as text
					set tgtText to trim(tgtText)
					if tgtText is not "" and not (tgtText contains "%") then
						set end of targetsList to tgtText
						set end of depsList to {tgtName:tgtText, deps:depsPart}
						
						if defaultTarget is "" and not (tgtText starts with ".") then
							set defaultTarget to tgtText
						end if
						
						set currentTarget to tgtText
					end if
				end repeat
			end if
		end if
	end repeat
	
	return {targets:targetsList, deps:depsList, recipes:recipesList, variables:variablesList, phonyTargets:phonyList, defaultTarget:defaultTarget}
end parseMakefile

-- Expand variables in a string (Classic Mac OS compatible)
on expandVars(str, variablesList)
	set result to str
	
	-- Expand $(VAR) format
	repeat while result contains "$("
		set startPos to offset of "$(" in result
		set afterParen to text (startPos + 2) thru -1 of result
		
		if afterParen contains ")" then
			set endPos to offset of ")" in afterParen
			set varName to text 1 thru (endPos - 1) of afterParen
			set varValue to ""
			
			-- Find variable value
			repeat with vInfo in variablesList
				if varName of vInfo is varName then
					set varValue to varValue of vInfo
					exit repeat
				end if
			end repeat
			
			if startPos > 1 then
				set beforeVar to text 1 thru (startPos - 1) of result
			else
				set beforeVar to ""
			end if
			
			if endPos < (length of afterParen) then
				set afterVar to text (endPos + 1) thru -1 of afterParen
			else
				set afterVar to ""
			end if
			
			set result to beforeVar & varValue & afterVar
		else
			exit repeat
		end if
	end repeat
	
	return result
end expandVars

-- Choose which target to build
on chooseTarget(makefileData)
	set targetsList to targets of makefileData
	set defaultTarget to defaultTarget of makefileData
	
	if defaultTarget is "" then
		display dialog "No targets found in Makefile!" buttons {"OK"} default button 1
		return ""
	end if
	
	if (count of targetsList) is 0 then
		return defaultTarget
	else if (count of targetsList) is 1 then
		return item 1 of targetsList
	else
		set targetChoice to choose from list targetsList with prompt Â
			"Select target to build:" default items {defaultTarget}
		
		if targetChoice is false then
			return ""
		else
			return item 1 of targetChoice
		end if
	end if
end chooseTarget

-- Build a target (simplified for Classic Mac OS)
on buildTarget(targetName, makefileData)
	set depsList to deps of makefileData
	set recipesList to recipes of makefileData
	set phonyList to phonyTargets of makefileData
	set variablesList to variables of makefileData
	set builtList to {}
	
	return buildTargetRecursive(targetName, makefileData, builtList)
end buildTarget

-- Recursive build helper
on buildTargetRecursive(targetName, makefileData, builtList)
	-- Already built?
	repeat with b in builtList
		if b is targetName then
			return true
		end if
	end repeat
	
	set depsList to deps of makefileData
	set recipesList to recipes of makefileData
	set phonyList to phonyTargets of makefileData
	set variablesList to variables of makefileData
	
	-- Get dependencies
	set targetDeps to ""
	repeat with d in depsList
		if tgtName of d is targetName then
			set targetDeps to deps of d
			exit repeat
		end if
	end repeat
	
	-- Build dependencies first
	if targetDeps is not "" then
		set depList to splitString(targetDeps, " ")
		repeat with dep in depList
			set depText to dep as text
			set depText to trim(depText)
			if depText is not "" then
				set depResult to buildTargetRecursive(depText, makefileData, builtList)
				if not depResult then
					return false
				end if
			end if
		end repeat
	end if
	
	-- Check if we need to build
	set needsBuild to false
	
	-- Check if it's a phony target
	repeat with p in phonyList
		if p is targetName then
			set needsBuild to true
			exit repeat
		end if
	end repeat
	
	-- If not phony, check if file exists
	if not needsBuild then
		set targetExists to fileExists(targetName)
		if not targetExists then
			set needsBuild to true
		end if
	end if
	
	-- Execute recipes if needed
	if needsBuild then
		set targetRecipes to {}
		repeat with r in recipesList
			if tgtName of r is targetName then
				set end of targetRecipes to r
			end if
		end repeat
		
		repeat with recipe in targetRecipes
			set cmd to cmd of recipe
			
			-- Expand variables in command
			set cmd to expandVars(cmd, variablesList)
			
			-- Expand automatic variables
			set cmd to replaceText(cmd, "$@", targetName)
			if targetDeps is not "" then
				set depList to splitString(targetDeps, " ")
				if (count of depList) > 0 then
					set firstDep to item 1 of depList
					set cmd to replaceText(cmd, "$<", trim(firstDep as text))
				end if
				set cmd to replaceText(cmd, "$^", targetDeps)
			end if
			
			-- Handle @ prefix (silent)
			set silent to false
			if cmd starts with "@" then
				set silent to true
				set cmd to text 2 thru -1 of cmd
			end if
			
			-- Handle - prefix (ignore errors)
			set ignoreError to false
			if cmd starts with "-" then
				set ignoreError to true
				set cmd to text 2 thru -1 of cmd
			end if
			
			if not silent then
				display dialog "Executing: " & cmd buttons {"OK"} default button 1 giving up after 2
			end if
			
			-- Note: On Classic Mac OS, actual execution would require MPW
			-- For now, we just show what would be executed
		end repeat
	end if
	
	set end of builtList to targetName
	return true
end buildTargetRecursive

-- Check if a file exists
on fileExists(fileName)
	tell application "Finder"
		try
			set scriptFolder to container of (path to me)
			set fileRef to file fileName of scriptFolder
			return true
		on error
			return false
		end try
	end tell
end fileExists

-- Utility: Split string by delimiter
on splitString(theString, theDelimiter)
	set oldDelimiters to AppleScript's text item delimiters
	set AppleScript's text item delimiters to theDelimiter
	set theList to text items of theString
	set AppleScript's text item delimiters to oldDelimiters
	return theList
end splitString

-- Utility: Trim whitespace
on trim(theString)
	if theString is "" then return ""
	set theString to trimLeft(theString)
	set theString to trimRight(theString)
	return theString
end trim

on trimLeft(theString)
	if theString is "" then return ""
	repeat while theString starts with " " or theString starts with "	"
		if (length of theString) > 1 then
			set theString to text 2 thru -1 of theString
		else
			return ""
		end if
	end repeat
	return theString
end trimLeft

on trimRight(theString)
	if theString is "" then return ""
	repeat while theString ends with " " or theString ends with "	"
		if (length of theString) > 1 then
			set theString to text 1 thru -2 of theString
		else
			return ""
		end if
	end repeat
	return theString
end trimRight

-- Utility: Replace text
on replaceText(theString, findString, replaceString)
	if findString is "" then return theString
	set oldDelimiters to AppleScript's text item delimiters
	set AppleScript's text item delimiters to findString
	set textItems to text items of theString
	set AppleScript's text item delimiters to replaceString
	set theString to textItems as text
	set AppleScript's text item delimiters to oldDelimiters
	return theString
end replaceText