
<!-- not use -->

<template>
	<view>
		<My-routeChange></My-routeChange>
		<uni-card title="会员卡" thumbnail="../../static/images/peopleInfo.png">
			<view class="cardList">
				<My-bikesInfo v-for="item in list" :cardInfo="item.attributes"></My-bikesInfo>
			</view>
		</uni-card>
		<view>
			<view>
			</view>
		</view>
		<button @click="lockNo">开锁</button>
		<button @click="lockOff">上锁</button>
		<button @click="buzzerOpen">喇叭开</button>
		<button @click="buzzerOff">喇叭关</button>
		<button @click="lightOpen">开灯</button>
		<button @click="lightOff">关灯</button>
	</view>
</template>

<script>
	import {
		mapState
	} from 'vuex'
	import {
		buzzer,
		led,
		fan,
		lock
	} from '../../utils/mqttControl.js'
	export default {
		data() {
			return {}
		},
		created() {
			this.$store.dispatch('list/dbList')
		},
		computed: {
			...mapState('list', ['list']),
			
		},
		methods: {
			buzzerOpen() {
				buzzer(1)
			},
			buzzerOff() {
				buzzer()
			},
			lightOpen(){
				led(1,1)
			},
			lightOff(){
				led()
			},
			lockOff() {
				lock()
			},
			lockNo() {
				lock(0)
			},


		},

	}
</script>

<style>
	.cardList {
		display: flex;
		flex-wrap: wrap;
		justify-content: space-around;
	}
</style>
