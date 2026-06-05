export default function myAlert(msg){
	// #ifdef H5
	alert(`${msg}`)
	// #endif
	// #ifdef APP
	uni.showToast({
		title:`${msg}`,
		icon:'none'
	})
	// #endif
}