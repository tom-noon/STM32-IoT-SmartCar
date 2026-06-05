<script>
	import {
		createConnection,
		doSubscribe
	} from './utils/mqtt.js'
	export default {
		onLaunch: function() {
			// this.$store.dispatch("temp/todayTemp1")
			// this.$store.dispatch("temp/todayTemp2")
			let temp1 = uni.getStorageSync('temp1List')
			// let temp2 = uni.getStorageSync('temp2List')
			let humi1 = uni.getStorageSync('humi1List')
			// let humi2 = uni.getStorageSync('humi2List')
			// let air = uni.getStorageSync('air')
			let spd = uni.getStorageSync('spd')
			
			
			
		// 	if(humi1){
		// 		this.$store.commit('humi/DBHUMIDATA', {
		// 			humi1,
		// 			humi2
		// 		})
		// 	}
		// 	if (temp1)
		// 		this.$store.commit('temp/DBTEMPDATA', {
		// 			temp1,
		// 			temp2
		// 		})
		// 	if(air){
		// 		this.$store.commit("air/BEGINAIR",air)
		// 	}
		// 	if(fan){
		// 		this.$store.commit("fan/BEGINFAN",JSON.parse(fan))
		// 	}
			
		},
		onShow: function() {
			createConnection()
			
			doSubscribe('testtopic/data')
			
		// 	doSubscribe('testtopic/takeCard')
		// 	doSubscribe('testtopic/addCard')
		// 	doSubscribe('testtopic/temp')
		// 	doSubscribe('testtopic/humi')
		// 	doSubscribe('testtopic/curTime')
		// 	doSubscribe("testtopic/fan")
		// 	doSubscribe("testtopic/air")
		// 	doSubscribe('testtopic/note')
		},
		onHide: function() {
			console.log('App Hide')
		}
	}
</script>

<style lang="scss">
	@import "colorui/main.css";
	@import "colorui/icon.css";
	@import "uview-ui/index.scss";
	@import url('./static/fonts/iconfont.css');
	;

	/*每个页面公共css */
	body {
		background-color: rgba(200, 200, 200, 0.2);
	}
</style>
