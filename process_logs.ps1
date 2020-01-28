<#
TITLE: process_logs.ps1
AUTHOR: Ben Barnard
DESCRIPTION: 
This is an incredibly lazy and inefficient PowerShell script to turn the BLE_Logger log files into defined durations.
Yes, it would be better to have the Arduino sketch manage the "enter" and "exit" events and log the durations directly.
For now, this will have to work. I wrote this in PowerShell initially because my end-users do not have access to Python.    
#>

#This is where you put the sensor collection files (CSVs)
$project_files_directory = "C:\Users\Documents\SensorFiles\"

#This is where you want to put the results
$results_file = "C:\Users\Documents\results.csv"

$files = get-childitem -Filter *.csv $project_files_directory
ForEach($file in $files)
    {
    $current_file = $file.FULLname
    $report = import-csv $current_file -Header sensor, beacon_name, beacon_uuid, rssi, timestamp
    $all_rooms = ($report | select sensor | sort-object -Property sensor -Unique).sensor 

    ForEach ($room in $all_rooms) #All of the rooms - this probably isn't necessary since all files are only for one room at a time
        {
        write-host "now examining" $room
        $room_log = $report | where-object {($_.sensor -like $room) -AND ($_.timestamp -inotlike "") } #All the records pertaining to the first room.
        $all_beacons = ($room_log | select beacon_name | sort-object -Property beacon_name -Unique).beacon_name # All the unique beacons that showed in that room
        ForEach ($beacon in $all_beacons)
            {
            $current_check = $null
            $start_duration = $null
            $last_check = $null
            write-host "now examining" $beacon
            #All the records pertaining to the first beacon, in the first room.
            $beacon_log = $room_log | where-object {$_.beacon_name -like $beacon } | sort-object  { $_."timestamp" -as [datetime] }

            $check_in_count = 1; #We're setting the increment value to 1 because if we've gotten this far, at least 1 must exist. 
            ForEach($log_item in $beacon_log)
            {
            #Loading the next line for evaluation
            $current_check = ($log_item.timestamp | get-date)
    
            #If we have no value in $start_duration, we must need to start a new period. Set $start_duration as $current_check
            #This will only ever happen at the beginning of room->beacon->records
            IF($start_duration -eq $null)
                {
                $start_duration = $current_check
                $last_check = $current_check
                $check_in_count++
                } 
            ELSE{
                #calculation to determine the time elapsed since the last check-in ($last_check)
                $minutes_difference = ($current_check - $last_check).totalminutes
                
                #If the time elapsed since the last check-in exceeds 4 minutes, we're calling it a duration and wrapping things up.
                IF(($minutes_difference -gt 4))
                    {
                    # write to file
                    $duration_minutes_difference = $last_check - $start_duration
                    #Write-Color -Color Green "FOUND ONE ($minutes_difference) - $room $beacon $start_duration $last_check, current_check was $current_check"
                    #write-host $beacon $minutes_difference ", `$start_duration" $start_duration $last_check $current_check
                    #log_to_file $room $beacon $start_duration $current_check $minutes_difference
                    $duration_result = New-Object PSObject
                    $duration_result | Add-Member -type NoteProperty -name room -Value $room
                    $duration_result | Add-Member -type NoteProperty -name beacon -Value $beacon
                    $duration_result | Add-Member -type NoteProperty -name start_duration -Value $start_duration
                    $duration_result | Add-Member -type NoteProperty -name current_check -Value $current_check
                    $duration_result | Add-Member -type NoteProperty -name last_check -Value $last_check
                    $duration_result | Add-Member -type NoteProperty -name minutes_difference -Value $minutes_difference
                    $duration_result | Add-Member -type NoteProperty -name duration_minutes_difference -Value $duration_minutes_difference
                    $duration_result | Add-Member -type NoteProperty -name check_in_count -Value $check_in_count
                    
                    #This is where we write the result to the CSV
                    #We **may** want to include an IF to only write back if $check_in_count > 2, otherwise we have to do it in post
                    $duration_result| Export-Csv -Path $results_file -NoTypeInformation -Append
                    $start_duration = $current_check
                    $last_check = $current_check
                    $current_check = $null
                    $check_in_count = 0
                    }
                ELSE
                    {
                    $last_check = $current_check
                    $check_in_count++
                    }
                }
            }
        
            write-host "no more records for " $beacon
            }
        write-host "no more records for " $room
        }

    }
