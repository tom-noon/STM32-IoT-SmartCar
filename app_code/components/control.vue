<!-- 控制模块 -->
<template>
	<uni-card title="控制台" thumbnail="../../static/images/control.png">
		<view class="controlBox">
			<view class="cu-form-group margin-top">
				<view class="title">控制模式</view>
				<view class="btn" style="display: flex;">
					<button @click="changeMode(0)">主动</button>
					<button @click="changeMode(1)">自动</button>
				</view>
			</view>
			
			<view class="cu-form-group">
				<view class="title">LED灯</view>
				<view class="btn" style="height: 50px;display: flex;">
					<button @click="changeLed(1)">开</button>
					<button @click="changeLed(0)">关</button>
				</view>
			</view>
			
			<view class="cu-form-group">
				<view class="title">控制方向</view>
				<view class="btn" style="display: flex;">
					<uni-number-box v-model="dir" :min="1" max="4" style="width: 200px;"></uni-number-box>
				</view>
			</view>
			<view class="cu-form-group">
				<view class="title">控制速度</view>
				<view class="btn" style="display: flex;">
					<uni-number-box v-model="speed" :max="4" style="width: 200px;"></uni-number-box>
				</view>
			</view>
			<view class="cu-form-group">
				<view class="title">运行时间</view>
				<view class="btn" style="display: flex;">
					<uni-number-box v-model="runtime" :max="10" style="width: 200px;"></uni-number-box>
				</view>
			</view>
			<button type="primary" @click="sendMqtt">执行</button>
			
		</view>
	</uni-card>
</template>
<script>
	import {doPublish} from '@/utils/mqtt'
	import {
		mapState
	} from 'vuex'
	import {
		mode,
		run,
		led
	} from '../utils/mqttControl.js'
		export default {
			data() {
				return {
					dir:0,
					speed:0,
					runtime:0
				}
			},
			created() {
			},
			computed: {
				...mapState('mode',['mode','temp','humi'])
			},
			methods: {
				changeMode(value){
					mode(value)
				},
				changeLed(value){
					led(value)
				},
				sendMqtt(){
					run(this.dir,this.speed,this.runtime*1000)
				},
			}
		}
</script>
<style>
	.windState {
		display: flex;
		text-align: center;
	}
	
	/* #ifdef H5 */
	.manu {
		display: flex;
		justify-content: space-around;
	}
	
	/* #endif */
	.manu view {
		display: flex;
	}
	
	.mode {
		display: flex;
		justify-content: space-evenly;
	}
	
	.mode view {
		display: flex;
	}
</style>
