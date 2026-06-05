const moment=require('moment')
export function toTime(stamp){
	return moment(stamp).format('MM/DD HH:mm:ss')
	
}
export function toStamp(time){
	return Date.parse(time)
}